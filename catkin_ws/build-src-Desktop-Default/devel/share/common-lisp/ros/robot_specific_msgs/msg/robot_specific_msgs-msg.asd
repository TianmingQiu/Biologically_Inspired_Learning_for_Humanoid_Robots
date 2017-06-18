
(cl:in-package :asdf)

(defsystem "robot_specific_msgs-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :std_msgs-msg
)
  :components ((:file "_package")
    (:file "TactileTouch" :depends-on ("_package_TactileTouch"))
    (:file "_package_TactileTouch" :depends-on ("_package"))
    (:file "JointState" :depends-on ("_package_JointState"))
    (:file "_package_JointState" :depends-on ("_package"))
    (:file "Bumper" :depends-on ("_package_Bumper"))
    (:file "_package_Bumper" :depends-on ("_package"))
    (:file "JointAnglesWithSpeed" :depends-on ("_package_JointAnglesWithSpeed"))
    (:file "_package_JointAnglesWithSpeed" :depends-on ("_package"))
  ))