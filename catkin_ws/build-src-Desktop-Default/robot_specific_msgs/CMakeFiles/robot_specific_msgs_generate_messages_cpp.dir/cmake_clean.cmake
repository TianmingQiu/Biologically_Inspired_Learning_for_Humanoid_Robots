FILE(REMOVE_RECURSE
  "CMakeFiles/robot_specific_msgs_generate_messages_cpp"
  "../devel/include/robot_specific_msgs/Bumper.h"
  "../devel/include/robot_specific_msgs/JointState.h"
  "../devel/include/robot_specific_msgs/JointAnglesWithSpeed.h"
  "../devel/include/robot_specific_msgs/TactileTouch.h"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/robot_specific_msgs_generate_messages_cpp.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
