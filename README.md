# Wanshift - A game prototype

## Demo Video 

[![Demo Video showing features and gameplay](https://i.imgur.com/1fNOHXR.png)](https://youtu.be/bOhlJTcyvFc)

## Table of Content

1.0 The Team<br>
1.1 Members<br>
2.0 Hardware/Software Requirements<br>
2.1 Introduction<br>
2.2 How the Game Works<br>
2.3 Controls<br>
3.0 Debug<br>
3.1 The Debug Console<br>
3.2 Domains<br>
	3.2.1 SRVDEBUG<br>
	3.2.2 GFX<br>
	3.2.3 SYSTEM<br>
	3.2.4 CAMERA<br>
3.3 General Debug Commands<br>
4.0 Intallation notes<br>

## 1.0 --- The Team ---

### 1.1 Members<br>
Rickard Fridvall<br>
Stefan Grahn<br>
Ossian Petri<br>
Emil Bertilsson<br>
Tobias Andersson<br>
Martin Säll<br>
Christopher Petersson<br>
Jimmy Gustafsson<br>

## 2.0 Hardware/Software Requirements
The game requires at minimum:<br>
*256 MB VRAM<br>
*DirectX 11<br>
*Shader Model 5<br>
*256 RAM<br>
*Windows Vista or later<br>
*500 MB disk space (not really though)<br>

## 2.1 Introduction
WanShift is a simple survival game. The player controls a robot
in a dynamically created world. The player is free to explore the
world at his/hers own leasure. The world is made up of 5 different
biomes, each with their own characteristics.

## 2.2 How the Game Works
The robot is driven by energy. The amount of energy stored is represented
by the glowing patterns on the robots clothes. A bright white light signals
a healthy storage of energy. A red light signals that only 10% of the energy
storage is left.

The light will pulsate as the energy is drained/harvested by external forces.

A default consumption is always draining the energy slowly. Certain actions
will increase the energy consumption. The following actions will increase
the energy drain:

*Sprinting<br>
*Using the headlight<br>
*Wisps (bright coloured moving blobs) attacks<br>

Energy can be restored by harvesting energy sources (bright blue floating blobs)

Additional energy drain is caused by moving from one biom to another. Different
biomes has different temperatures and the robot will slowly acclimatize by its own.
During this period the robot will drain at a higher rate. Acclimatizing will also 
allow the robot to move at a slightly higher pace.


## 2.3 Controls
The game uses the following controls:

*Movement - WASD<br>
*Energy Harvest - 'e' (in close proximity to a energy resource)<br>
*Headlights - 'f'<br>
*Jump - SPACE<br>
*Sprint - Left Shift (during movement)<br>
*Exit - ESC<br>

## 3.0 --- Debug ---

### 3.1 The Debug Console
Open the debug console by pressing '§'

### 3.2 Domains
In order to switch domains type 'switch domainName', where 'domainName' is one of
the following names:

* srvdebug<br>
* gfx<br>
* system<br>
* camera<br>

### 3.2.1 SRVDEBUG
Shows different textures in a separarate field. Used for debugging texture generation
and rendertargets. type 'show name' where 'name' corresponds to the handle name of that
resource.

### 3.2.2 GFX
Shows:<br>
* Current Biom<br>
* Biom blend values<br>
* Total amount of used VRAM<br>
* Amount of used VRAMfor ID3D11Buffers<br>
* Amount of used VRAMfor ID3D11Texture2D + ID3D11Texture3D + BackBuffer<br>
* Amount of used VRAMfor Shaderfiles<br>

### 3.2.3 SYSTEM
Shows:<br>
* Total amount of currently used RAM<br>
* Largest RAM peak<br>
* FPS<br>

### 3.2.4 CAMERA
Shows:<br>
* Camera Mode (free / locked)<br>
* Avatar Position<br>
* Camera Position<br>
* Time of Day not working correctly in the current build)<br>

### 3.2 General Debug Commands
* Debug Camera - 'cam free'<br>
* Game Camera - 'cam lock'<br>
(please note that due to a bug the user must first write 'cam lock' then<br>
'cam free' in order to switch to debug camera)<br>

## 4.0 Installation Notes
Double click the installation .exe and let install. Double click WanShift.exe in
the installation folder in order to play.
