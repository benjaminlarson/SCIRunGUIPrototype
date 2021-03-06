/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2012 Scientific Computing and Imaging Institute,
   University of Utah.

   License for the specific language governing rights and limitations under
   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
*/

#include <Core/Algorithms/Base/AlgorithmPreconditions.h>
#include <Core/Algorithms/Math/ReportMatrixInfo.h>
#include <Core/Algorithms/Base/AlgorithmVariableNames.h>
#include <Core/Datatypes/MatrixTypeConversions.h>
#include <Core/Datatypes/DenseMatrix.h>
#include <Core/Datatypes/SparseRowMatrix.h>
#include <Core/Datatypes/DenseColumnMatrix.h>
#include <iostream>

using namespace SCIRun::Core::Algorithms::Math;
using namespace SCIRun::Core::Datatypes;
using namespace SCIRun::Core::Algorithms;


struct NumberOfElements : MatrixVisitor
{
  size_t value() const { return value_; }
  
  NumberOfElements() : value_(0) {}
  size_t value_;

  virtual void visit(DenseMatrix& m)
  {
    value_ = m.rows() * m.cols();
  }

  virtual void visit(DenseColumnMatrix& m)
  {
    value_ = m.nrows();
  }

  virtual void visit(SparseRowMatrix& m)
  {
    value_ = m.nonZeros();
  }
};

struct MinimumCoefficient : MatrixVisitor
{
  double value() const { return value_; }

  MinimumCoefficient() : value_(0) {}
  double value_;

  virtual void visit(DenseMatrix& m)
  {
    if (!m.empty())
      value_ = m.minCoeff();
  }

  virtual void visit(DenseColumnMatrix& m)
  {
    if (!m.empty())
      value_ = m.minCoeff();
  }

  virtual void visit(SparseRowMatrix& m)
  {
    if (!m.empty())
      value_ = *std::min_element(m.valuePtr(), m.valuePtr() + m.nonZeros());
  }
};

struct MaximumCoefficient : MatrixVisitor
{
  double value() const { return value_; }

  MaximumCoefficient() : value_(0) {}
  double value_;

  virtual void visit(DenseMatrix& m)
  {
    if (!m.empty())
      value_ = m.maxCoeff();
  }

  virtual void visit(DenseColumnMatrix& m)
  {
     if (!m.empty())
       value_ = m.maxCoeff();
  }

  virtual void visit(SparseRowMatrix& m)
  {
    if (!m.empty())
      value_ = *std::max_element(m.valuePtr(), m.valuePtr() + m.nonZeros());
  }
};

ReportMatrixInfoAlgorithm::Outputs ReportMatrixInfoAlgorithm::run(const Inputs& input) const
{
  ENSURE_ALGORITHM_INPUT_NOT_NULL(input, "Null input matrix");

  const std::string type = matrix_is::whatType(input);

  NumberOfElements num;
  input->accept(num);
  MinimumCoefficient min;
  input->accept(min);
  MaximumCoefficient max;
  input->accept(max);

  return Outputs(type, 
    input->nrows(), 
    input->ncols(), 
    num.value(),
    min.value(),
    max.value()
    );
}

AlgorithmOutput ReportMatrixInfoAlgorithm::run_generic(const AlgorithmInput& input) const
{
  auto matrix = input.get<Matrix>(Variables::InputMatrix);

  auto outputs = run(matrix);

  AlgorithmOutput output;
  output.setTransient(outputs); 
  return output;
}

std::string ReportMatrixInfoAlgorithm::summarize(const Outputs& info)
{
  std::ostringstream ostr;
  ostr << "Type:\t" << info.get<0>() << "\n"
    << "# Rows:\t" << info.get<1>() << "\n"
    << "# Columns:\t" << info.get<2>() << "\n"
    << "# Elements:\t" << info.get<3>() << "\n"
    << "Minimum:\t" << info.get<4>() << "\n"
    << "Maximum:\t" << info.get<5>() << "\n";
  return ostr.str();
}