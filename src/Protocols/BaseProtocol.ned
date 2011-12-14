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

package drgsimlib.Protocols;

//
// BaseProtocol is the base implementation of the IProtocol interface.
// Protocol implementations should extend BaseProtocol.
//
// @author Kristjan V. Jonsson
// @version 0.9
// @date december 2011
//
// @todo document
//
simple BaseProtocol like IProtocol
{
	parameters:
        int nodeid;
    gates:
        input  lowerIn;
        output lowerOut;    
        input  auxIn @loose;      
        output auxOut @loose; 
        input  lowerControlIn;
        output lowerControlOut;
}
