# kf.py updated file for robot estimation assignment
# project[2]
# (C) 2017 David Feil-Seifer
# Modified by Bryce Monaco, Fall 2018


import numpy as np
import math
from scipy import stats
import scipy.stats

# kf_update: update state estimate [u, sigma] with new control [xdot] and measurement [z]
# 	parameters:
#			u : 2x1 vector with state estimate (x) at time t-1 and control (xdot) at time t
#				u[0]=state estimate (x_t-1)
#				u[1]=velocity (xdot)
#			sigma: 2x2 matrix with covariance at time t-1
#			z (int): observed (uncertain) measurement of state (x) at time t
#	returns: [u sigma] updated state with estimate at time t

def kf_update(u, sigma, z):
	# Variable setup
	delta_t = 1  # Need to find this value
	little_sigma_x = 5
	little_sigma_z = 10
	
	u_now = u  # Technically not needed, but helps keep everything in the same format

	big_h = np.matrix([1, 0])
	big_g = np.matrix([delta_t * delta_t, delta_t / 2]).T  # G = [delta_t^2; delta_t/2]

	big_sigma_x = np.linalg.matrix_power(big_g * big_g.T, little_sigma_x)
	big_sigma_z = little_sigma_z * little_sigma_z
	big_sigma_now = sigma

	big_f = np.matrix([[1, delta_t], [0, 1]])

	big_k_next = ((big_f * big_sigma_now * big_f.T) + big_sigma_x) * big_h.T * np.linalg.inv(big_h * ((big_f * big_sigma_now * big_f.T) + big_sigma_x) * big_h.T + big_sigma_z)
	big_sigma_next = (np.identity(2) - (big_k_next * big_h)) * ((big_f * big_sigma_now * big_f.T) + big_sigma_x)
	u_next = big_f * u_now + big_k_next * (z - big_h * big_f * u_now)  # Note: unsure if z or z_t+1. Book says z_t+1 but idk how to get that

	return [u_next, big_sigma_next]


# door_update: update estimate of door locations
# 	parameters:
#			u : 2x1 vector with state estimate (x) at time t-1 and control (xdot) at time t-1
#			sigma: 2x2 matrix with covariance at time t-1
#			d (binary): door sensor at time t-1 
#			door_dist (array of size 10): probability (0..1) that a door exists at each location (0..9)
#	returns: [door_dist] updated door distribution

count_array = [[1,2],[1,2],[1,2],[1,2],[1,2],[1,2],[1,2],[1,2],[1,2],[1,2]]  # 2D array of counts for each door, [#counts there is a door, #counts there is not a door]

def door_update(u, sigma, d, door_dist):
	door_index = int(u[0]/100)  # Used so we know which door's probability to change

	"""
	D = There is a door
	S = The sensor is correct
	P(S)=0.6*0.8=0.48		The door sensor is correct, 48% of the time overall
	P(S|D)=0.6				The door sensor says true when there is a door 60% of the time
	P(D|S)=P(S|D)*P(D)/P(S)
	P(D|S)=0.6*P(D)/0.48=1.25*P(D)=1.25*(#times d/#times !d)
	"""

	if d is True:
		count_array[door_index][0] = count_array[door_index][0] + 1  # Inc the count of times d is true
	else:
		count_array[door_index][1] = count_array[door_index][1] + 1 # Inc the count of times d is false

	prob_d = count_array[door_index][0] / count_array[door_index][1]

	# Update the prob for the door we are currently at, leave all others the same
	door_dist[door_index] = np.clip(1.25*prob_d, 0, 1)  # This shouldn't need to be clipped (clamped) but floating point error sometimes makes it 1.00000001 which makes the red bar fill the entire vertical space above it

	return door_dist


















