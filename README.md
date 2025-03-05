<p align="center">
	<img src="https://github.com/user-attachments/assets/3ec3a600-c2a5-4257-8442-768025ea8545">
	<br><br>
	<img src="https://img.shields.io/github/actions/workflow/status/Wolf49406/Dota2Patcher/msbuild.yml?style=for-the-badge&logo=appveyor&labelColor=282a36&color=ff79c6">
	<img src="https://img.shields.io/github/downloads/Wolf49406/Dota2Patcher/total?style=for-the-badge&logo=appveyor&labelColor=282a36&color=ff79c6">
</p>

## ❓ How To:
* Download [latest release](https://github.com/Wolf49406/Dota2Patcher/releases/latest)
* Launch Dota2Patcher.exe
* Set your settings
* Launch Dota 2
	* Patcher will wait for a lobby to start
* That's all!

## ❗ Features:

### ✨ NEW: River Changer
Set any pyte of river (aka [River Vials](https://dota2.fandom.com/wiki/River_Vials))

### ✨ NEW: Weather Changer
Set any type of weather: `Snow, Rain, Moonbeam, Pestilence, Harvest, Sirocco, Spring, Ash, Aurora`  

![weather](https://github.com/user-attachments/assets/54b801b6-eb5a-427b-abce-f3268780d759)

### ✨ NEW: Visible By Enemy  
Draws a ring under your hero if you are visible by enemy

![image](https://github.com/user-attachments/assets/051de205-f4fb-440b-b04a-81db3c65bca6)

### ✨ NEW: Enemy Illusions Detection

![image](https://github.com/user-attachments/assets/68fd753a-8f16-439f-9bfa-54542169a9a9)

### ✨ Camera distance patch without ConVars
Now you don't have to use ConVars (ex: `dota_camera_distance 1500; r_farz 18000`) to set camera distance!  
Dota2Patcher will do it for you. Default value is 1500 which is pretty safe to use.  
But you still can use `dota_camera_distance` if you want to.

### ✨ fog_enable patch
Disable fog (`fog_enable 0`) without ConVars!

### ✨ set_rendering_enabled
Remember removed `dota_use_particle_fow` convar? So this is it but better.  
Shows ALL hidden particles in FOW: Enemy TP's, spells, even jungle farm particles (blood splashes).  

![particles](https://i.ibb.co/L08kLBZ/photo-2025-01-04-23-41-17.jpg)

### ✨ sv_cheats unlock
Allows you to use any cheat-protected commands in multiplayer (ex: `dota_camera_distance 1500`) WITHOUT `sv_cheats`

### Popular convars (cheat commands):
* `fog_enable`: default `1`
	* `0` - Remove fog
		* ex. `fog_enable 0`
* `fow_client_nofiltering`: default `0`
	* `1` - Remove anti-aliasing of fog
		* ex. `fow_client_nofiltering 1`
* `dota_camera_distance`: default `1200`
	* `*any number*` - change camera distance
		* ex. `dota_camera_distance 1500`
* `r_farz`: default `-1`
	* `18000` - Override the far clipping plane
	* You need multiply x2 of camera distance or just set `18000`
		* ex. `dota_camera_distance 1500` -> `r_farz 3000`
* `dota_range_display`: default `0`
	* `*any number*` - Displays a ring around the hero at the specified radius
		* ex. `dota_range_display 1200`
* `cl_weather`: default `0`
	* `*any number*`(1-10) - Change weather
		* ex. `cl_weather 8`
