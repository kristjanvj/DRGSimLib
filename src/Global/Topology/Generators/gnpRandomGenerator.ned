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
// GNP random graph generator
//
// Implements a dynaic variant of the classical G_np graph. See the networkx manual for reference on random 
// graphs. 
// The graph evolves dynamically as nodes are added. New nodes are connected
// to one partner (selected uniformly at random) with probability one. Additional edges
// to the remainder of the vertex set are added with probability p.
//
// @author Kristjan V. Jonsson
// @version 0.9
// @date december 2011
//
// @todo document
//
simple GnpRandomGenerator extends BasicGenerator
{
    parameters:
        volatile double p = default(0.2);
        volatile double linkChurnProbability = default(0.0); // The per minute link churn probability
    @class(GnpRandomGenerator);
}
