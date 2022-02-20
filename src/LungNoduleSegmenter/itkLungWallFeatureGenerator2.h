/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkLungWallFeatureGenerator2.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef itkLungWallFeatureGenerator2_h
#define itkLungWallFeatureGenerator2_h

#include "itkFeatureGenerator.h"
#include "itkImage.h"
#include "itkImageSpatialObject.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkVotingBinaryHoleFillFloodingImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

namespace itk
{

/** \class LungWallFeatureGenerator2
 * \brief Generates a feature image 
 *
 * The typical use of this class would be to generate the feature image needed
 * by a Level Set filter to internally compute its speed image. This
 * transformation is very close to a simply thresholding selection on the input
 * image, but with the advantage of a smooth transition of intensities.
 *
 * SpatialObjects are used as inputs and outputs of this class.
 *
 * \ingroup SpatialObjectFilters
 * \ingroup LesionSizingToolkit
 */
template <unsigned int NDimension>
class ITK_EXPORT LungWallFeatureGenerator2 : public FeatureGenerator<NDimension>
{
public:
  ITK_DISALLOW_COPY_AND_ASSIGN(LungWallFeatureGenerator2);

  /** Standard class type alias. */
  using Self = LungWallFeatureGenerator2;
  using Superclass = FeatureGenerator<NDimension>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(LungWallFeatureGenerator2, FeatureGenerator);

  /** Dimension of the space */
  static constexpr unsigned int Dimension = NDimension;

  /** Type of spatialObject that will be passed as input to this
   * feature generator. */
  using InputPixelType = signed short;
  using InputImageType = Image< InputPixelType, Dimension >;
  using InputImageSpatialObjectType = ImageSpatialObject< NDimension, InputPixelType >;
  using InputImageSpatialObjectPointer = typename InputImageSpatialObjectType::Pointer;
  using SpatialObjectType = typename Superclass::SpatialObjectType;

  /** Input data that will be used for generating the feature. */
  using ProcessObject::SetInput;
  void SetInput( const SpatialObjectType * input );
  const SpatialObjectType * GetInput() const;

  /** Output data that carries the feature in the form of a
   * SpatialObject. */
  const SpatialObjectType * GetFeature() const;

  /** Set the Hounsfield Unit value to threshold the Lung. */
  itkSetMacro( LungThreshold, InputPixelType );
  itkGetMacro( LungThreshold, InputPixelType );

	/** Set the structuring element radius (in mm). Default 3mm */
	itkSetMacro(Radius, double);
	itkGetMacro(Radius, double);

	/** Use the GPU ? */
	itkSetMacro(UseGPU, bool);
	itkGetMacro(UseGPU, bool);
	itkBooleanMacro(UseGPU);

protected:
  LungWallFeatureGenerator2();
  ~LungWallFeatureGenerator2() override;
  void PrintSelf(std::ostream& os, Indent indent) const override;

  /** Method invoked by the pipeline in order to trigger the computation of
   * the segmentation. */
  void  GenerateData () override;

private:
  using InternalPixelType = float;
  using InternalImageType = Image< InternalPixelType, Dimension >;

  using OutputPixelType = float;
  using OutputImageType = Image< OutputPixelType, Dimension >;

  using OutputImageSpatialObjectType = ImageSpatialObject< NDimension, OutputPixelType >;

	using MaskImageType = Image< unsigned char, 3 >;
	typedef typename MaskImageType::Pointer MaskImagePointer;
	using ThresholdFilterType = BinaryThresholdImageFilter<
    InputImageType, MaskImageType >;
	using RescaleFilterType = RescaleIntensityImageFilter<
		MaskImageType, OutputImageType >;
	using ThresholdFilterPointer = typename ThresholdFilterType::Pointer;
	using RescaleFilterPointer = typename RescaleFilterType::Pointer;
	typedef typename InputImageType::SizeType          SizeType;

	using VotingHoleFillingFilterType = VotingBinaryHoleFillFloodingImageFilter<
		MaskImageType, MaskImageType >;
  using VotingHoleFillingFilterPointer = typename VotingHoleFillingFilterType::Pointer;

  ThresholdFilterPointer                m_ThresholdFilter;
  VotingHoleFillingFilterPointer        m_VotingHoleFillingFilter;
	RescaleFilterPointer                  m_RescaleFilter;

  InputPixelType                        m_LungThreshold;
	double m_Radius;

	MaskImagePointer DoCurvatureConstrainedSmoothing(MaskImageType *, SizeType holeFillRadiusPx);
	MaskImagePointer DoCurvatureConstrainedSmoothingCPU(MaskImageType *, SizeType holeFillRadiusPx);
#ifdef USE_GPU
	MaskImagePointer DoCurvatureConstrainedSmoothingGPU(MaskImageType *, SizeType holeFillRadiusPx);
#endif

	bool m_UseGPU;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
# include "itkLungWallFeatureGenerator2.hxx"
#endif

#endif
