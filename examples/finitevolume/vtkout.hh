// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
/*
  This file is part of the eWoms project.

  eWoms is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  eWoms is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with eWoms.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <dune/grid/io/file/vtk/vtkwriter.hh>

#include <stdio.h>

template<class G, class V>
void vtkout (const G& grid, const V& c, const char* name, int k, double time=0.0, int rank=0)
{
  Dune::VTKWriter<typename G::LeafGridView> vtkwriter(grid.leafGridView());
  char fname[128];
  char sername[128];
  sprintf(fname,"%s-%05d",name,k);
  sprintf(sername,"%s.series",name);
  vtkwriter.addCellData(c,"celldata");
  vtkwriter.write(fname,Dune::VTK::ascii);
  if ( rank == 0)
  {
    std::ofstream serstream(sername, (k==0 ? std::ios_base::out : std::ios_base::app));
    serstream << k << " " << fname << ".vtu " << time << std::endl;
    serstream.close();
  }
}
