#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkConnectedThresholdImageFilter.h"
#include <iostream>

typedef itk::Image<short, 2> ImageType;
typedef itk::ImageFileReader<ImageType> ReaderType;
typedef itk::ImageFileWriter<ImageType> WriterType;
typedef itk::ConnectedThresholdImageFilter<ImageType, ImageType> FilterType;

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

int main(int argc, char const *argv[])
{
  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  reader->SetFileName(argv[1]);
  writer->SetFileName("output.dcm");

  FilterType::Pointer filter = FilterType::New();

  int lower = inputToInt(argv[2]);
  int upper = inputToInt(argv[3]);

  filter->SetLower(lower);
  filter->SetUpper(upper);

  // parse x and y coordinates args
  float x = inputToFloat(argv[4]);
  float y = inputToFloat(argv[5]);

  // parse intensity
  int intensity = inputToInt(argv[6]);

  filter->SetReplaceValue(intensity);

  FilterType::IndexType index;
  index[0] = x;
  index[1] = y;

  ImageType::Pointer image = reader->GetOutput();

  filter->SetSeed(index);
  filter->SetInput(reader->GetOutput());
  filter->Update();
  writer->SetInput(filter->GetOutput());
  writer->Update();

  return 0;
}