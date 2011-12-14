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

package drgsimlib.Global.Topology;

//
// Topology control module
//
// Used with the Topology compound module to manage a dynamic random graph topology. 
//
// @author Kristjan V. Jonsson
// @version 0.9
// @date december 2011
//
// @see Topology
// @see TopologyControlNIC
//
// @todo document
//
simple TopologyControl
{
	parameters:
	    int updateInterval @unit("s") = default(1s);
	    bool saveSnapshot = default(false);
	    bool recordDuringInitialization = default(false);
	    string snapshotFile = default("");
	    string logFileName = default("");
}
