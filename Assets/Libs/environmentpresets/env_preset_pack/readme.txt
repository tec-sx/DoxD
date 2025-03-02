Complete information about these assets and support can be found in the documentation here:
https://docs.cryengine.com/display/CEMANUAL/Environment+Editor+Use+Cases

Copy these environment presets into your project's libs\environmentpresets folder to use them in your levels.

IMPORTANT: Time of day must be set correctly in the environment editor's Curve Editor. Time of Day is written in all preset filenames except those which should be set to 1200. Correct keyframe variables are ONLY set at the specified times of day; nothing has been configured at any other time of day except day_night_cycle_2.env.

night_clear_skies.env:
A simple clear night with a low moon in the sky

dawn_1630.env:
A “magic hour” preset that can be used for dawn or sunset, with the sun near the horizon and warm, and stars still just visible in the brightening sky.

dawn_fast_clouds_0630.env:
Another :magic hour” preset with the sun at the horizon, light levels still low, stars still visible, and a strong influence on volumetric clouds.

midday_1630.env:
Sun moderately high in the sky; could be used for any time around midday (albeit not near the equator).

day_night_cycle_2.env:
A full 24 hour day/night cycle, with all elements adjusted for key times of day, Could be used for dynamic time of day or simply as one preset that can be switched to any of the keyframed times to dramatically change looks.

desert_gold_magic_hour_1630.env:
Sun low and warm in the sky, heavy volumetric clouds configured to create a clearing or approaching storm look, with strong colors.

foggy_dense.env:
Dense global volumetric fog and shadows softened, light directly overhead as if diffused by cloud cover.

night_dark_clear.env:
A clear, starry night with a warm moon.

nightvision.env:
A simulated infrared camera view with a green tint, heavy grain and bloom, and limited depth of field. Intended to be used with light components.

magic_hour_low_clouds.env:
Volumetric clouds placed low to mimic fog volumes, strongly affected by wind.

craters.env:
A night preset meant to be used with light components and/or emissive fog volumes.

overcast_1800.env:
A special case example meant to be used with a dense, global, emissive fog volume and light components; no sunlight is used. Sky is left bright.

sidescroller_silhouettes.env:
A stylized example: monochrome, no sunlight or skylight, heavy grain and depth of field blur. Meant to be used with an emissive global fog volume and/or light components to provide illumination or the illusion of it.
