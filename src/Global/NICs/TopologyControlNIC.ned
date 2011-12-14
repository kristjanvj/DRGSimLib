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

package drgsimlib.Global.NICs;

//
// TopologyControlNIC
//
// NIC module which works in conjunction with the Topology compound object to
// enable communications over a random graph topology
//
// @author Kristjan V. Jonsson
// @version 0.9
// @date december 2011
//
// @see Topology
// @see TopologyControl
//
// @todo Make this module compatible with the INET modules in terms of gates and control.
//
simple TopologyControlNIC
{
    parameters:
        int dataRate @unit("Kbps") = default(10Kbps);
 		volatile double bitErrorRate = default(0.0); // Zero: lossless
        volatile double processingDelay @unit("s") = default(truncnormal(0.1s,0.1s));
        volatile double propagationDelay @unit("s") = default(truncnormal(0.05s,0.05s));
    gates:
        input  upperIn;             // Input from upper layers
        output upperOut;            // Output to upper layers
        input  networkIn @directIn; // Direct input for messages
        input  controlIn @directIn; // Direct input for topology events
        input  upperControlIn;      // Control gate connected to upper layers
        output upperControlOut;     // Control gate connected to upper layers
}
