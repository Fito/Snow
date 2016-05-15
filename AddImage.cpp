#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkAddImageFilter.h"
#include <iostream>

typedef itk::Image<short, 3> ImageType;
typedef itk::ImageFileReader<ImageType> ReaderType;
typedef itk::ImageFileWriter<ImageType> WriterType;
typedef itk::AddImageFilter <ImageType, ImageType > AddImageFilterType;

int main(int argc, char const *argv[])
{
  ReaderType::Pointer reader1 = ReaderType::New();
  ReaderType::Pointer reader2 = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  reader1->SetFileName(argv[1]);
  reader2->SetFileName(argv[2]);
  writer->SetFileName("output.dcm");

  AddImageFilterType::Pointer addFilter = AddImageFilterType::New ();
  addFilter->SetInput1(reader1->GetOutput());
  addFilter->SetInput2(reader2->GetOutput());
  addFilter->Update();

  writer->SetInput(addFilter->GetOutput());
  writer->Update();

  return 0;
}
