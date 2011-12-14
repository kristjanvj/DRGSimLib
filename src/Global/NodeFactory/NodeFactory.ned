// ****************************************************************************
//
// DRGSim -- Dynamic Random Graph Simulation Components
//
// ****************************************************************************
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License version 3
// as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not see <http://www.gnu.org/licenses/>.
//

//
// Node factory module
//
// The node factory instantiates nodes dynamically during a simulation 
// run from a tracefile.
//
// Based on NodeFactory from the Opposim project 
//
// @author Kristjan V. Jonsson
// @version 0.9
// @date december 2011
//
package drgsimlib.Global.NodeFactory;

//
// NodeFactory
//
// Dynamically generates and deletes nodes in a running simulation.
//
// @author Kristjan V. Jonsson
// @version 0.9
// @date december 2011
//
simple NodeFactory
{
	parameters:
        string nodeTypename;                              // The OMNeT++ class name for generated objects
        string nodePrefix;                                // A string prefix to name generated nodes                                
        int initialPopulation = default(0);   
		int maxPopulation = default(0);                   // Zero: Unlimited
		string logFileName = default("");
        volatile double generateInterval = default(0.0);
        volatile double lifetime = default(0.0);          // Zero: unlimited lifetime
        double minLifetime @unit("s") = default(0.0s);    // Zero: no limit
}

