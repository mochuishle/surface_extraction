#! /usr/bin/env python

from __future__ import print_function, division

import rospy
import dynamic_reconfigure.client

if __name__ == "__main__":
    rospy.init_node('emit_current_time')

    client = dynamic_reconfigure.client.Client('filter')

    rospy.sleep(4)

    start = rospy.get_time()

    rate = rospy.Rate(10)  # Hz

    speedup_factor = 1

    print("Starting filter_limit expansion")

    while not rospy.is_shutdown():
        config = client.update_configuration({'filter_limit_max': (rospy.get_time() - start) * speedup_factor})

        rate.sleep()