# Copyright (c) Accumetra, LLC
#pragma once

#include "itkImage.h"
#include "itkResampleImageFilter.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkIdentityTransform.h"

namespace supersample
{

/**
 * Supersamples a volume.
 * 
 * Precision:
 *    InputPixelType is the precision of the input pixel
 *    TPrecision is the precision of the internal computations (for resampling)
 *    TOutputPixelType is the output pixel precision
 *
 * Nyquist criterion
 * (a) If the isotropicSpacing desired is more than the in-plane spacing but less than the
 *     out of plane spacing, then care must be taken to resample along the in-plane axis through
 *     a kernel (gaussian kernel) is used. The standard deviation of the kernel is the desired
 *     output resolution (ie isotropicSpacing) as per Nyquist. The resulting image is then linearly
 *     interpolated to provide the final output
 * (b) If the isotropicSpacing desired is less than the in-plane and out-of-plane spacing, then no
 *     such step is necessary and the input image is linearly interpolated to provide the final output
 *
 * Default parameters
 * (a) If 'supersample' is false, nothing is done. The output is simply the input casted based on the
 *     desired TOutputPixelType
 * (b) If 'supersample' is true, but 'isotropicSpacing' is not specified, then the output spacing is
 *     chosen as the sqrt of the L2 norm of the in-plane and out-of-plane spacing.
 * (c) If 'supersanmple' is true and the 'isotropicSpacing' is specified, it is use. 
 *
**/
template< class InputPixelType = short, class TPrecision = float, class TOutputPixelType = float >
class SupersampleVolume
{

public:
  typedef TOutputPixelType OutputPixelType;
  typedef TPrecision InternalPixelType;
  typedef itk::Image< InputPixelType, 3 > InputImageType;
  typedef itk::Image< InternalPixelType, 3 > InternalImageType;
  typedef itk::Image< OutputPixelType, 3 > OutputImageType;
  typedef typename OutputImageType::Pointer OutputImagePointer;

  static OutputImagePointer Execute( const InputImageType * in, bool supersample = true, double isotropicSpacing = 0)
  {
    typedef TOutputPixelType OutputPixelType;
    typedef TPrecision InternalPixelType;
    typedef itk::Image< InputPixelType, 3 > InputImageType;
    typedef itk::Image< InternalPixelType, 3 > InternalImageType;
    typedef itk::Image< OutputPixelType, 3 > OutputImageType;

    typedef itk::CastImageFilter< InputImageType, InternalImageType > CastFilterType;
    typename CastFilterType::Pointer castFilter = CastFilterType::New();
    castFilter->SetInput(in);
    castFilter->Update();

    typename InternalImageType::Pointer inputImage;

    const typename InputImageType::SpacingType& inputSpacing = in->GetSpacing();
    const double isoSpacing = (isotropicSpacing == 0 ? 
      std::sqrt( inputSpacing[2] * inputSpacing[0] ) : isotropicSpacing);

    // This is the case where we are supersampling along Z but subsampling along X and Y.
    // In this case, we need to apply a gaussian in-plane as per Nyquist criterion.
    // Let us handle this specially
    if (supersample && (inputSpacing[0] < isotropicSpacing && inputSpacing[1] < isotropicSpacing))
    {
      typedef itk::RecursiveGaussianImageFilter<
        InternalImageType, InternalImageType > GaussianFilterType;
      typename GaussianFilterType::Pointer smootherX = GaussianFilterType::New();
      typename GaussianFilterType::Pointer smootherY = GaussianFilterType::New();
      smootherX->SetInput( castFilter->GetOutput() );
      smootherY->SetInput( smootherX->GetOutput() );
      smootherX->SetSigma( isoSpacing );
      smootherY->SetSigma( isoSpacing );
      smootherX->SetDirection( 0 );
      smootherY->SetDirection( 1 );
      smootherY->Update();
      inputImage = smootherY->GetOutput();
    }
    else
    {
      inputImage = castFilter->GetOutput();
    }

    inputImage->DisconnectPipeline();
    castFilter = NULL;

    if (!supersample)
    {
      return inputImage;
    }

    typedef itk::ResampleImageFilter< InternalImageType, OutputImageType > ResampleFilterType;
    typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();
    typedef itk::IdentityTransform< double, 3 > TransformType;
    TransformType::Pointer transform = TransformType::New();
    transform->SetIdentity();
    resampler->SetTransform( transform );

    typedef itk::LinearInterpolateImageFunction< InternalImageType, double > InterpolatorType;
    typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
    resampler->SetInterpolator( interpolator );
    resampler->SetDefaultPixelValue( -1000 );

    typename OutputImageType::SpacingType spacing;
    spacing[0] = isoSpacing;
    spacing[1] = isoSpacing;
    spacing[2] = isoSpacing;
    resampler->SetOutputSpacing( spacing );
    resampler->SetOutputOrigin( inputImage->GetOrigin() );
    resampler->SetOutputDirection( inputImage->GetDirection() );

    typename InputImageType::SizeType inputSize = inputImage->GetLargestPossibleRegion().GetSize();
    typedef typename InputImageType::SizeType::SizeValueType SizeValueType;
    const double dx = inputSize[0] * inputSpacing[0] / isoSpacing;
    const double dy = inputSize[1] * inputSpacing[1] / isoSpacing;
    const double dz = (double)(inputSize[2] - 1 ) * inputSpacing[2] / isoSpacing;
    typename InputImageType::SizeType   size;
    size[0] = static_cast<SizeValueType>( dx );
    size[1] = static_cast<SizeValueType>( dy );
    size[2] = static_cast<SizeValueType>( dz );

    resampler->SetSize( size );
    resampler->SetInput( inputImage );
    resampler->Update();

    typename OutputImageType::Pointer out = resampler->GetOutput();
    out->DisconnectPipeline();

    return out;
  }

};

}
