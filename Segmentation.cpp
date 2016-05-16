#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkConnectedThresholdImageFilter.h"
#include "itkConfidenceConnectedImageFilter.h"
#include <iostream>

typedef itk::Image<short, 3> ImageType;
typedef itk::ImageFileReader<ImageType> ReaderType;
typedef itk::ImageFileWriter<ImageType> WriterType;
typedef itk::ConnectedThresholdImageFilter<ImageType, ImageType> CTFilterType;
typedef itk::ConfidenceConnectedImageFilter<ImageType, ImageType> CCFilterType;

float inputToFloat(std::string input) {
  float convertedInput = 0.0;
  std::stringstream ssX;
  ssX << input;
  ssX >> convertedInput;
  return convertedInput;
}

int inputToInt(std::string input) {
  int convertedInput = 0;
  std::stringstream ssX;
  ssX << input;
  ssX >> convertedInput;
  return convertedInput;
}

class Segmentator
{
  private:
    int intensity, multiplier, radius;
    CCFilterType::IndexType CCseed;
    CCFilterType::Pointer CCfilter;

    CTFilterType::IndexType CTseed;
    CTFilterType::Pointer CTfilter;
    ImageType *inputImage;

  public:
    Segmentator(ImageType *inputImage) {
      this->inputImage = inputImage;
      this->CCfilter = CCFilterType::New();
      this->CTfilter = CTFilterType::New();
    }

    void setSeedPoint(float x, float y, float z) {
      CCseed[0] = x; CCseed[1] = y; CCseed[2] = z;
      CTseed[0] = x; CTseed[1] = y; CTseed[2] = z;
    }

    void setIntersity(int intensity) {
      this->intensity = intensity;
    }

    void setMultiplier(int multiplier) {
      this->multiplier = multiplier;
    }

    void setRadius(int radius) {
      this->radius = radius;
    }

    ImageType * confidenceConnectedSegment() {
      CCfilter->SetReplaceValue(intensity);
      CCfilter->SetSeed(CCseed);
      CCfilter->SetInput(inputImage);

      CCfilter->SetInitialNeighborhoodRadius(radius);
      CCfilter->SetMultiplier(multiplier);
      CCfilter->SetNumberOfIterations(0);

      CCfilter->Update();

      return CCfilter->GetOutput();
    }

    ImageType * connectedThresholdSegment(int lowerBound, int upperBound) {
      CTfilter->SetReplaceValue(intensity);
      CTfilter->SetSeed(CTseed);
      CTfilter->SetInput(inputImage);

      CTfilter->SetLower(lowerBound);
      CTfilter->SetUpper(upperBound);

      CTfilter->Update();
      return CTfilter->GetOutput();
    }
};

// Usage Example:
// ./Segmentation cttopet_dcm.dcm 2.2 3 258 230 344 255

int main(int argc, char const *argv[])
{
  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  // input file
  reader->SetFileName(argv[1]);
  writer->SetFileName("output.dcm");

  // parse x, y and z coordinates args
  float x = inputToFloat(argv[4]);
  float y = inputToFloat(argv[5]);
  float z = inputToFloat(argv[6]);

  // parse intensity replacement value
  int intensity = inputToInt(argv[7]);

  // lower bound when Threshold, multiplier when Region Growing
  int param2 = inputToFloat(argv[2]);

  // upper bound when Threshold, radius when Region Growing
  int param3 = inputToFloat(argv[3]);

  char segmentationType = *argv[8];

  // instantiate a segmentator with the input image
  Segmentator * segmentator = new Segmentator(reader->GetOutput());

  // configure segmentation parameters on segmentator
  segmentator->setSeedPoint(x, y, z);
  segmentator->setIntersity(intensity);
  segmentator->setMultiplier(param2);
  segmentator->setRadius(param3);

  ImageType *resultImage;

  if(segmentationType == 'R') {
    // run confidence connected (region growing) segmentation
    resultImage = segmentator->confidenceConnectedSegment();
  } else if (segmentationType == 'T') {
    // run connected threshold (thresholding) segmentation
    resultImage = segmentator->connectedThresholdSegment(param2, param3);
  } else {
    printf("Segmentation type must be either R(egion growin) or T(hresholding)\n");
    exit(0);
  }

  ImageType::SpacingType spacing =  resultImage->GetSpacing();

  printf("x Spacing %f\n", spacing[0]);
  printf("y Spacing %f\n", spacing[1]);
  printf("z Spacing %f\n", spacing[2]);

  itk::ImageRegionIterator<ImageType> it(resultImage, resultImage->GetRequestedRegion());
  it.GoToBegin();

  int regionPixels = 0;
  while( !it.IsAtEnd() ) {
    if(it.Get()) { regionPixels++; }
    ++it;
  }

  double volume = regionPixels * spacing[0] * spacing[1];
  printf("Volume: %f\n", volume);

  writer->SetInput(resultImage);
  writer->Update();

  return 0;
}
