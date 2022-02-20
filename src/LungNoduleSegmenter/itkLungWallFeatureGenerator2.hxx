/*=========================================================================


=========================================================================*/
#ifndef itkLungWallFeatureGenerator2_hxx
#define itkLungWallFeatureGenerator2_hxx

#include "itkLungWallFeatureGenerator2.h"
#include "itkProgressAccumulator.h"
#include "itkTimeProbe.h"
#ifdef USE_GPU
#include "itkGPUIterativeHoleFiller.h"
#endif


namespace itk
{

/**
 * Constructor
 */
template <unsigned int NDimension>
LungWallFeatureGenerator2<NDimension>
::LungWallFeatureGenerator2() : m_Radius(3.0)
{
#ifdef USE_GPU
	m_UseGPU = true;
#else
	m_UseGPU = false;
#endif
	
	this->SetNumberOfRequiredInputs( 1 );
  this->SetNumberOfRequiredOutputs( 1 );

  this->m_ThresholdFilter = ThresholdFilterType::New();
  this->m_VotingHoleFillingFilter = VotingHoleFillingFilterType::New();
	m_RescaleFilter = RescaleFilterType::New();

  this->m_ThresholdFilter->ReleaseDataFlagOn();
  this->m_VotingHoleFillingFilter->ReleaseDataFlagOn();

  typename OutputImageSpatialObjectType::Pointer outputObject = OutputImageSpatialObjectType::New();

  this->ProcessObject::SetNthOutput( 0, outputObject.GetPointer() );

  this->m_LungThreshold = -400;
}


/*
 * Destructor
 */
template <unsigned int NDimension>
LungWallFeatureGenerator2<NDimension>
::~LungWallFeatureGenerator2()
{
}

template <unsigned int NDimension>
void
LungWallFeatureGenerator2<NDimension>
::SetInput( const SpatialObjectType * spatialObject )
{
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput(0, const_cast<SpatialObjectType *>( spatialObject ));
}

template <unsigned int NDimension>
const typename LungWallFeatureGenerator2<NDimension>::SpatialObjectType *
LungWallFeatureGenerator2<NDimension>
::GetFeature() const
{
  if (this->GetNumberOfOutputs() < 1)
    {
    return nullptr;
    }

  return static_cast<const SpatialObjectType*>(this->ProcessObject::GetOutput(0));

}


/*
 * PrintSelf
 */
template <unsigned int NDimension>
void
LungWallFeatureGenerator2<NDimension>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "Lung threshold " << this->m_ThresholdFilter << std::endl;
}


/*
 * Generate Data
 */
template <unsigned int NDimension>
void
LungWallFeatureGenerator2<NDimension>
::GenerateData()
{
  typename InputImageSpatialObjectType::ConstPointer inputObject =
    dynamic_cast<const InputImageSpatialObjectType * >( this->ProcessObject::GetInput(0) );

  if( !inputObject )
    {
    itkExceptionMacro("Missing input spatial object");
    }

  const InputImageType * inputImage = inputObject->GetImage();

  if( !inputImage )
    {
    itkExceptionMacro("Missing input image");
    }

  // Report progress.
  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);
  progress->RegisterInternalFilter( this->m_ThresholdFilter, 0.1 );
  progress->RegisterInternalFilter( this->m_VotingHoleFillingFilter, 0.9 );

  this->m_ThresholdFilter->SetInput( inputImage );
  this->m_VotingHoleFillingFilter->SetInput( this->m_ThresholdFilter->GetOutput() );

  this->m_ThresholdFilter->SetLowerThreshold( this->m_LungThreshold );
  this->m_ThresholdFilter->SetUpperThreshold( 3000 );

  this->m_ThresholdFilter->SetInsideValue( 0 );
  this->m_ThresholdFilter->SetOutsideValue( 255 );
	m_ThresholdFilter->Update();

  typename InternalImageType::SizeType  ballManhattanRadius;
	for (unsigned int i = 0; i < NDimension; ++i)
	{
		ballManhattanRadius[i] = std::ceil(m_Radius / inputImage->GetSpacing()[i]);
		if (ballManhattanRadius[i] < 3)
		{
			ballManhattanRadius[i] = 3;
		}
	}
	
	MaskImagePointer mask = m_ThresholdFilter->GetOutput();
	
	//std::cout << "ballManhattanRadius " << ballManhattanRadius << std::endl;
	MaskImagePointer binaryWall = this->DoCurvatureConstrainedSmoothing(
		mask.GetPointer(),
		ballManhattanRadius);

	m_RescaleFilter->SetInput(binaryWall);
	m_RescaleFilter->SetOutputMinimum(0.0);
	m_RescaleFilter->SetOutputMaximum(1.0);
	m_RescaleFilter->Update();

	//std::cout << "Used " << this->m_VotingHoleFillingFilter->GetCurrentIterationNumber() << " iterations " << std::endl;
  //std::cout << "Changed " << this->m_VotingHoleFillingFilter->GetTotalNumberOfPixelsChanged() << " pixels " << std::endl;

  typename OutputImageType::Pointer outputImage = this->m_RescaleFilter->GetOutput();

  outputImage->DisconnectPipeline();

  auto * outputObject = dynamic_cast< OutputImageSpatialObjectType * >(this->ProcessObject::GetOutput(0));

  outputObject->SetImage( outputImage );
}

template <unsigned int NDimension>
typename LungWallFeatureGenerator2<NDimension>::MaskImagePointer
LungWallFeatureGenerator2<NDimension>::
DoCurvatureConstrainedSmoothing(MaskImageType *m, SizeType holeFillRadiusPx)
{
#ifdef USE_GPU
	if (m_UseGPU)
	{
		return DoCurvatureConstrainedSmoothingGPU(m, holeFillRadiusPx);
	}
	else
#endif
	{
		return DoCurvatureConstrainedSmoothingCPU(m, holeFillRadiusPx);
	}
}


template <unsigned int NDimension>
typename LungWallFeatureGenerator2<NDimension>::MaskImagePointer
LungWallFeatureGenerator2<NDimension>::
DoCurvatureConstrainedSmoothingCPU(MaskImageType *m, SizeType holeFillRadiusPx)
{
	itk::TimeProbe tp;
	tp.Start();

	//std::cout << "  Postprocess fill holes (CPU version)..." << std::endl;
	typename VotingHoleFillingFilterType::Pointer filler =
		VotingHoleFillingFilterType::New();
	filler->SetInput(m);
	filler->SetRadius(holeFillRadiusPx);
	//std::cout << "holeFillRadiusPx: " << holeFillRadiusPx << std::endl;
	filler->SetForegroundValue(255);
	filler->SetBackgroundValue(0);
	filler->SetMajorityThreshold(1);
	filler->SetMaximumNumberOfIterations(1000);
	filler->Update();

	tp.Stop();
	//std::cout << "Hole filling Time (CPU): " << tp.GetTotal() << std::endl;

	MaskImagePointer o = filler->GetOutput();
	o->DisconnectPipeline();
	return o;
}

#ifdef USE_GPU
template <unsigned int NDimension>
LungWallFeatureGenerator2<NDimension>::MaskImagePointer
LungWallFeatureGenerator2<NDimension>::
DoCurvatureConstrainedSmoothingGPU(MaskImageType *m, SizeType holeFillRadiusPx)
{
	itk::TimeProbe tp;
	tp.Start();

	typedef itk::GPUImage< unsigned char, 3 >	GPUMaskImageType;
	typedef typename GPUMaskImageType::Pointer GPUMaskImagePointer;
	typedef itk::CastImageFilter<MaskImageType, GPUMaskImageType>
		CPUToGPUInputImageCasterType;
	typedef itk::CastImageFilter<GPUMaskImageType, MaskImageType >
		GPUOutputToOutputImageCasterType;

	// binary mask to GPU image
	CPUToGPUInputImageCasterType::Pointer maskToGPUMaskCaster =
		CPUToGPUInputImageCasterType::New();
	maskToGPUMaskCaster->SetInput(m);
	maskToGPUMaskCaster->Update();

	GPUMaskImageType::Pointer im = maskToGPUMaskCaster->GetOutput();

	// Run the GPU filter in a loop for desired number of iterations...
	typedef itk::GPUIterativeHoleFiller< GPUMaskImageType > HoleFillingFilterType;
	HoleFillingFilterType::Pointer filler;

	const unsigned int maxIter = 1000;
	for (unsigned int i = 0; i < maxIter; i++)
	{
		filler = HoleFillingFilterType::New();
		filler->SetRadius(holeFillRadiusPx);
		filler->SetMajorityThreshold(1);
		filler->SetInput(im);
		filler->Update();

		// copy from GPU to CPU
		filler->GetOutput()->UpdateBuffers();

		im = filler->GetOutput();
		im->DisconnectPipeline();
	}

	typedef itk::CastImageFilter< GPUMaskImageType, MaskImageType > GPUToCPUCastImageFilterType;
	typename GPUToCPUCastImageFilterType::Pointer casterGPUToCPU = GPUToCPUCastImageFilterType::New();
	casterGPUToCPU->SetInput(im);
	casterGPUToCPU->Update();
	MaskImagePointer cpuBinaryImage = casterGPUToCPU->GetOutput();
	cpuBinaryImage->DisconnectPipeline();

	tp.Stop();
	//std::cout << "Hole filling Time (GPU): " << tp.GetTotal() << std::endl;

	// Free up 
	maskToGPUMaskCaster = NULL;

	return cpuBinaryImage;
}
#endif

} // end namespace itk

#endif
