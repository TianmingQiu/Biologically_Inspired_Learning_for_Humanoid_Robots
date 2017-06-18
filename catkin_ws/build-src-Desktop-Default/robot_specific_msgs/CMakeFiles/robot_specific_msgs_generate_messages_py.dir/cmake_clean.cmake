FILE(REMOVE_RECURSE
  "CMakeFiles/robot_specific_msgs_generate_messages_py"
  "../devel/lib/python2.7/dist-packages/robot_specific_msgs/msg/_Bumper.py"
  "../devel/lib/python2.7/dist-packages/robot_specific_msgs/msg/_JointState.py"
  "../devel/lib/python2.7/dist-packages/robot_specific_msgs/msg/_JointAnglesWithSpeed.py"
  "../devel/lib/python2.7/dist-packages/robot_specific_msgs/msg/_TactileTouch.py"
  "../devel/lib/python2.7/dist-packages/robot_specific_msgs/msg/__init__.py"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/robot_specific_msgs_generate_messages_py.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
