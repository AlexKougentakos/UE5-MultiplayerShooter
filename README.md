# Multiplayer Shooter Game

![](https://i.imgur.com/RccZhy8.jpeg)

This is a game I made as a learning project, while following a Udemy course on Networking in Unreal Engine 5. I did that in order to better understand networking, as well as C++ inside Unreal Engine. Despite the fact that I followed a course, I still abided by my programming principles that I was tought and strayed from the course when I deemed necessary.


# Game Features

## Weapons
There are 7 different weapons in the game, which consist of both projectile and hit scan weapons.

![](https://i.imgur.com/dJLWnZP.png)

Weapons include: **Assault Riffle** (Projectile), **Pistol** (Hit Scan), **Shotgun** (Hit Scan), **Submachine Gun** (Hit Scan), **Rocket Launcher** (Projectile), **Grenade Launcher** (Projectile) & **Sniper Riffle** (Hit Scan)

## Power Ups
There are 4 different power-ups in the game.
There are spawners for these power-ups which randomly choose one to spawn. There are also spawners which spawn a fixed power up.

![](https://i.imgur.com/G2UPuqK.png)

Power-ups include: Health, Shields, Jump Boost, Speed Boost

## Ammo
Similar to power ups there exist ammo spawners next to where you originally found the weapon. These increase your carried ammo, which get's reset every time you die.
Each weapon has a unique ammo type.

![](https://i.imgur.com/CgBL91a.png)


# Design Choices & Implementations
## Multiplayer Funcitonality
In order to achieve multiplayer functionality I took advantage of the Steam multiplayer Subsystem in Unreal Engine. I did not intend on publishing this game on steam so it does not have its own steam app id, so I am still using the publicly available development app id: 480.

## Server Side Rewind
Server side rewind is a technique used in many games in order to mitigate the effect of lag and offer the players a smoother and fairer experience. In my game it's currently only implemented for the shooting (not including rocket & grenade launchers) aspect.

How it works is that every frame, the lag compensation component on each character stores their hit boxes, on the server of course, and when the client get's a hit they tell the server. The server then rewinds to the position in time when the shot happened, checks to see if the player was there, interpolates between the previous and next frame if needed and performs the line trace, letting the client know the result.
For extremely high pings this is disabled as it would cause players to die even after taking cover.

<div style="display: flex; justify-content: space-between;">
 <img src="https://i.imgur.com/asflkC9.png" width="48%" />
  <img src="https://i.imgur.com/97RoyLz.png" width="48%" />
</div>

## Assets
Non of the assets are my own. The majority where aquired in the Unreal Engine marketplace from previous Epic Games projects and migrates to this one. Some where edited to fit my needs and some I found on the internet.

