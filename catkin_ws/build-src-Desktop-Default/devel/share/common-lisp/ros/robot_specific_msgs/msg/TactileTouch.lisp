; Auto-generated. Do not edit!


(cl:in-package robot_specific_msgs-msg)


;//! \htmlinclude TactileTouch.msg.html

(cl:defclass <TactileTouch> (roslisp-msg-protocol:ros-message)
  ((button
    :reader button
    :initarg :button
    :type cl:fixnum
    :initform 0)
   (state
    :reader state
    :initarg :state
    :type cl:fixnum
    :initform 0))
)

(cl:defclass TactileTouch (<TactileTouch>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <TactileTouch>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'TactileTouch)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name robot_specific_msgs-msg:<TactileTouch> is deprecated: use robot_specific_msgs-msg:TactileTouch instead.")))

(cl:ensure-generic-function 'button-val :lambda-list '(m))
(cl:defmethod button-val ((m <TactileTouch>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_specific_msgs-msg:button-val is deprecated.  Use robot_specific_msgs-msg:button instead.")
  (button m))

(cl:ensure-generic-function 'state-val :lambda-list '(m))
(cl:defmethod state-val ((m <TactileTouch>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_specific_msgs-msg:state-val is deprecated.  Use robot_specific_msgs-msg:state instead.")
  (state m))
(cl:defmethod roslisp-msg-protocol:symbol-codes ((msg-type (cl:eql '<TactileTouch>)))
    "Constants for message type '<TactileTouch>"
  '((:BUTTONFRONT . 1)
    (:BUTTONMIDDLE . 2)
    (:BUTTONREAR . 3)
    (:STATERELEASED . 0)
    (:STATEPRESSED . 1))
)
(cl:defmethod roslisp-msg-protocol:symbol-codes ((msg-type (cl:eql 'TactileTouch)))
    "Constants for message type 'TactileTouch"
  '((:BUTTONFRONT . 1)
    (:BUTTONMIDDLE . 2)
    (:BUTTONREAR . 3)
    (:STATERELEASED . 0)
    (:STATEPRESSED . 1))
)
(cl:defmethod roslisp-msg-protocol:serialize ((msg <TactileTouch>) ostream)
  "Serializes a message object of type '<TactileTouch>"
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'button)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'state)) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <TactileTouch>) istream)
  "Deserializes a message object of type '<TactileTouch>"
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'button)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'state)) (cl:read-byte istream))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<TactileTouch>)))
  "Returns string type for a message object of type '<TactileTouch>"
  "robot_specific_msgs/TactileTouch")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'TactileTouch)))
  "Returns string type for a message object of type 'TactileTouch"
  "robot_specific_msgs/TactileTouch")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<TactileTouch>)))
  "Returns md5sum for a message object of type '<TactileTouch>"
  "b75165bf9dfed26d50ad4e3162304225")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'TactileTouch)))
  "Returns md5sum for a message object of type 'TactileTouch"
  "b75165bf9dfed26d50ad4e3162304225")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<TactileTouch>)))
  "Returns full string definition for message of type '<TactileTouch>"
  (cl:format cl:nil "# From ROS package from Uni Freiburg~%# A message for Nao's tactile interface (touch buttons on the head)~%~%uint8 button            # which of the three segments is touched~%uint8 state             # pressed or released, see below~%~%uint8 buttonFront=1~%uint8 buttonMiddle=2~%uint8 buttonRear=3~%~%uint8 stateReleased=0~%uint8 statePressed=1~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'TactileTouch)))
  "Returns full string definition for message of type 'TactileTouch"
  (cl:format cl:nil "# From ROS package from Uni Freiburg~%# A message for Nao's tactile interface (touch buttons on the head)~%~%uint8 button            # which of the three segments is touched~%uint8 state             # pressed or released, see below~%~%uint8 buttonFront=1~%uint8 buttonMiddle=2~%uint8 buttonRear=3~%~%uint8 stateReleased=0~%uint8 statePressed=1~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <TactileTouch>))
  (cl:+ 0
     1
     1
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <TactileTouch>))
  "Converts a ROS message object to a list"
  (cl:list 'TactileTouch
    (cl:cons ':button (button msg))
    (cl:cons ':state (state msg))
))
