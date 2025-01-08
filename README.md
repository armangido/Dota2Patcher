# Dota2Patcher
 The return of legendary Dota2Patcher. Will add more features later

### How To Install:
* Download [latest release](https://github.com/Wolf49406/Dota2Patcher/releases/latest)
* Launch Dota2Patcher.exe
* Launch Dota 2
* Wait for Patcher to do his job
* That's all!

## New Feature: set_rendering_enabled
Remember removed `dota_use_particle_fow` convar? So this is it but better.  
Shows ALL hidden particles in FOW: Enemy TP's, spells, even jungle farm particles (blood splashes).  

![image](https://i.ibb.co/L08kLBZ/photo-2025-01-04-23-41-17.jpg)

## sv_cheats unlock
Allows you to use any cheat-protected commands in multiplayer (ex: `dota_camera_distance 1500`)

### Popular convars (cheat commands):
* `sv_cheats`: default `0`
	* `1` - Unlock convars thats have cheat flag (use it first!)
		* ex. `sv_cheats 1`
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
sv_cheats 1; fog_enable 0; fow_client_nofiltering 1; dota_camera_distance 1500; r_farz 3000;
```
