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

package drgsimlib.Protocols.GAP;
import drgsimlib.Protocols.BaseProtocol;

//
// GAP -- an implementation of the Generic Aggregation Protocol as
// described by Dam and Stadler (2005) with minor modifications.
// One modification is the query message which is used in this 
// implmentation to bootstrap the aggregation system.
//
// Extends BaseProtocol
//
// @author Kristjan V. Jonsson
// @version 0.9
// @date december 2011
//
// @todo document
//
simple GAP extends BaseProtocol
{
    parameters:
        string recordingFileName = default("");
        string logFileName = default("");
        double updateDelay @unit("s") = default(120s);
        bool oneshotMode = default(true);
        double oneshotDelay @unit("s") = default(10s);
	@class(gap);
}
