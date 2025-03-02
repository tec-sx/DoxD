#pragma once

//! Interpolate float.
inline void Interpolate(float& actual, float goal, float speed, float frameTime, float limit = 0)
{
	float delta(goal - actual);

	if (limit > 0.001f)
	{
		delta = max(min(delta, limit), -limit);
	}

	actual += delta * min(frameTime * speed, 1.0f);
}

//! Interpolate float - wraps at minValue/maxValue.
inline void InterpolateWrapped(float& actual, float goal, float minValue, float maxValue, float speed, float frameTime, float limit = 0)
{
	assert(minValue < maxValue);
	assert(minValue <= goal);
	assert(goal <= maxValue);
	assert(minValue <= actual);
	assert(actual <= maxValue);

	float range = maxValue - minValue;
	float movement = 0.0f;
	if (goal < actual)
	{
		if (actual - goal > range * 0.5f)
			movement = goal + range - actual;
		else
			movement = goal - actual;
	}
	else
	{
		if (goal - actual > range * 0.5f)
			movement = goal - (actual - range);
		else
			movement = goal - actual;
	}

	if (limit > 0.001f)
	{
		movement = max(min(movement, limit), -limit);
	}

	actual += movement * min(frameTime * speed, 1.0f);
}