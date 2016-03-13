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
    CTFilterType::IndexType CTseed;
    ImageType *inputImage;

  public:
    Segmentator(ImageType *inputImage) {
      this->inputImage = inputImage;
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
      CCFilterType::Pointer filter = CCFilterType::New();
      filter->SetReplaceValue(intensity);
      filter->SetSeed(seed);
      filter->SetInput(inputImage);

      filter->SetInitialNeighborhoodRadius(radius);
      filter->SetMultiplier(multiplier);
      filter->SetNumberOfIterations(0);

      filter->Update();
      return filter->GetOutput();
    }

    ImageType * ConnectedThresholdSegment(int lowerBound, int upperBound) {
      CTFilterType::Pointer filter = CTFilterType::New();
      filter->SetReplaceValue(intensity);
      filter->SetSeed(seed);
      filter->SetInput(inputImage);

      filter->SetLower(lowerBound);
      filter->SetUpper(upperBound);

      filter->Update();
      return filter->GetOutput();
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

  // intensity threshold
  // int lower = inputToInt(argv[2]);
  // int upper = inputToInt(argv[3]);

  // parse x, y and z coordinates args
  float x = inputToFloat(argv[4]);
  float y = inputToFloat(argv[5]);
  float z = inputToFloat(argv[6]);

  // parse intensity replacement value
  int intensity = inputToInt(argv[7]);
  int multiplier = inputToInt(argv[2]);
  int radius = inputToInt(argv[3]);

  // instantiate a segmentator with the input image
  Segmentator * segmentator = new Segmentator(reader->GetOutput());

  // configure segmentation parameters on segmentator
  segmentator->setSeedPoint(x, y, z);
  segmentator->setIntersity(intensity);
  segmentator->setMultiplier(multiplier);
  segmentator->setRadius(radius);

  // run confidence connected (region growing) segmentation
  writer->SetInput(segmentator->confidenceConnectedSegment());
  writer->Update();

  // run connected threshold segmentation
  // writer->SetInput(segmentator->confidenceConnectedSegment(lowerBound, upperBound));
  // writer->Update();

  return 0;
}
