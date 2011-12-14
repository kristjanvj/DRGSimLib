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

package drgsimlib.Nodes;
import drgsimlib.Protocols.IProtocol;
import drgsimlib.Global.NICs.TopologyControlNIC;

//
// A generic node compound module for use in a DRG simulation.
// Contains a generic protocol module and a TopologyControlNIC.
// Plug in a protocol such as Ping in the ini file.
//
// @author Kristjan V. Jonsson
// @version 0.9
// @date december 2011
//
// @todo document
//
module Node
{
    parameters:
        int nodeid;
        string protocolName = default("Ping");
    submodules:
        nic: TopologyControlNIC {
            parameters:
                @display("p=175,43;i=block/ifcard");
        }
        protocol: <protocolName> like IProtocol {
            parameters:
                nodeid = nodeid;
                @display("p=59,43;i=block/cogwheel");
        }
    connections:
        nic.upperOut --> protocol.lowerIn;
        protocol.lowerOut --> nic.upperIn;
        nic.upperControlOut --> protocol.lowerControlIn;
        protocol.lowerControlOut --> nic.upperControlIn;
}
