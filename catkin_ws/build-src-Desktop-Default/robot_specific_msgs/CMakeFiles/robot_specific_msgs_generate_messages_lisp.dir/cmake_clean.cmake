FILE(REMOVE_RECURSE
  "CMakeFiles/robot_specific_msgs_generate_messages_lisp"
  "../devel/share/common-lisp/ros/robot_specific_msgs/msg/Bumper.lisp"
  "../devel/share/common-lisp/ros/robot_specific_msgs/msg/JointState.lisp"
  "../devel/share/common-lisp/ros/robot_specific_msgs/msg/JointAnglesWithSpeed.lisp"
  "../devel/share/common-lisp/ros/robot_specific_msgs/msg/TactileTouch.lisp"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/robot_specific_msgs_generate_messages_lisp.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
