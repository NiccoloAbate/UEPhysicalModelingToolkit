Physical Modeling Library Metasounds Unreal Engine Plugin

Please see https://github.com/NiccoloAbate/UEPhysicalModelingToolkit for full documentation.

Installation:
Create a "Plugins" folder in the root of your project and copy the CM151MSLib folder into the new folder. Now reopen your project.
*Only tested on Windows system.

Metasound Nodes:
Cos and Sin nodes (Audio | Float)
Subtract (Float From Audio)
Subtract (Audio From Float)
Divide (Audio)
Divide (Float By Audio)
Expression (Audio | Float) - nodes for parsing and computing arbitrary expressions from string representatation, using https://github.com/codeplea/tinyexpr to compile expressions.
	moth operators "+", "-", "/", "*", and "%"
	variable "x"
	available functions min, max, clamp, abs, acos, asin, atan, atan2, ceil, cos, cosh, exp, floor, ln (calls to log), log (calls to log10), log10, pow, sin, sinh, sqrt, tan, tanh

Metasound Graph Nodes:
Wrap (Audio) - just a graph containing the expression x - floor(x)