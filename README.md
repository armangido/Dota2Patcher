# Dota2Patcher
 The return of legendary Dota2Patcher  
 
[![MSBuild](https://github.com/Wolf49406/Dota2Patcher/actions/workflows/msbuild.yml/badge.svg?branch=main)](https://github.com/Wolf49406/Dota2Patcher/actions/workflows/msbuild.yml)

![image](https://github.com/user-attachments/assets/93bd5129-2224-41aa-80c9-a69aff4d5c32)

## ❓ How To:
* Download [latest release](https://github.com/Wolf49406/Dota2Patcher/releases/latest)
* Launch Dota2Patcher.exe
* Set your settings
* Launch Dota 2
	* Patcher will wait for a lobby to start
* Wait for Patcher to do his job
	* Patcher will close if no errors occurred
* That's all!

### 🧾 Documentation:  

Developers can look at [documentation](https://wolf49406.github.io/Dota2Patcher/) (work in progress)  
Разработчики могут посмотреть [документацию](https://wolf49406.github.io/Dota2Patcher/) (дополняется)  

## ❗ Features:

### ✨ New: Camera distance patch without ConVars
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

### Raw

* Just copy raw list and past to console

```
fog_enable 0; dota_camera_distance 1500; r_farz 3000; fow_client_nofiltering 1;
```
