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
	
	u_now = u

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

def door_update(u, sigma, d, door_dist):
	return door_dist


















