/* ************************************************************************
 * Copyright (c) 2018 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * ************************************************************************ */

#include <iostream>
#include <cstdlib>

#include <rocalution.hpp>

using namespace rocalution;

int main(int argc, char* argv[]) {

  if (argc == 1) { 
    std::cerr << argv[0] << " <matrix> [Num threads]" << std::endl;
    exit(1);
  }

  init_rocalution();

  if (argc > 2) {
    set_omp_threads_rocalution(atoi(argv[2]));
  } 

  info_rocalution();

  LocalVector<double> x;
  LocalVector<double> rhs;

  LocalMatrix<double> mat;

  mat.ReadFileMTX(std::string(argv[1]));

  //  mat.MoveToAccelerator();
  //  x.MoveToAccelerator();
  //  rhs.MoveToAccelerator();

  x.Allocate("x", mat.GetN());
  rhs.Allocate("rhs", mat.GetM());

  // Linear Solver
  GMRES<LocalMatrix<double>, LocalVector<double>, double > ls;

  // Preconditioner
  BlockPreconditioner<LocalMatrix<double>, LocalVector<double>, double > p;
  Solver<LocalMatrix<double>, LocalVector<double>, double > **p2;

  int n = 2;
  int *size;
  size = new int[n];

  p2 = new Solver<LocalMatrix<double>, LocalVector<double>, double >*[n];

  for (int i=0; i<n; ++i) {
    size[i] = mat.GetM() / n ;   

    MultiColoredILU<LocalMatrix<double>, LocalVector<double>, double > *mc;
    mc = new MultiColoredILU<LocalMatrix<double>, LocalVector<double>, double >;
    p2[i] = mc;


    //    AMG<LocalMatrix<double>, LocalVector<double>, double > *amg;
    //    amg = new AMG<LocalMatrix<double>, LocalVector<double>, double >;
    //    amg->InitMaxIter(2);
    //    amg->Verbose(0);
    //    p2[i] = amg;

  }

  double tick, tack;
  
  rhs.Ones();
  x.Zeros(); 

  p.Set(n, 
        size,
        p2);
  p.SetDiagonalSolver();

  ls.SetOperator(mat);
  ls.SetPreconditioner(p);
  //  ls.Verbose(2);

  ls.Build();

  mat.Info();

  tick = rocalution_time();

  ls.Solve(rhs, &x);

  tack = rocalution_time();
  std::cout << "Solver execution:" << (tack-tick)/1000000 << " sec" << std::endl;

  ls.Clear();

  for (int i=0; i<n; ++i) {
    delete p2[i];
    p2[i] = NULL;
  }

  delete[] size;
  delete[] p2;
  p2 = NULL;

  stop_rocalution();

  return 0;
}
