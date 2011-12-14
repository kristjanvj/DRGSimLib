// ****************************************************************************
//
// DRGSim -- Dynamic Random Graph Simulation Components
//
// ****************************************************************************
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package drgsimlib.Global.Topology.Generators;

//
//
// @author Kristjan V. Jonsson
// @version 0.9
// @date december 2011
//
// @todo document
//
simple RandomNeighborGenerator extends BasicGenerator
{
    parameters:
        double alpha = default(1.0);
        double beta  = default(2.0);
        volatile double linkChurnProbability = default(0.0); // default: no relinking
    @class(RandomNeighborGenerator);
}
