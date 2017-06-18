; Auto-generated. Do not edit!


(cl:in-package robot_specific_msgs-msg)


;//! \htmlinclude Bumper.msg.html

(cl:defclass <Bumper> (roslisp-msg-protocol:ros-message)
  ((bumper
    :reader bumper
    :initarg :bumper
    :type cl:fixnum
    :initform 0)
   (state
    :reader state
    :initarg :state
    :type cl:fixnum
    :initform 0))
)

(cl:defclass Bumper (<Bumper>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <Bumper>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'Bumper)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name robot_specific_msgs-msg:<Bumper> is deprecated: use robot_specific_msgs-msg:Bumper instead.")))

(cl:ensure-generic-function 'bumper-val :lambda-list '(m))
(cl:defmethod bumper-val ((m <Bumper>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_specific_msgs-msg:bumper-val is deprecated.  Use robot_specific_msgs-msg:bumper instead.")
  (bumper m))

(cl:ensure-generic-function 'state-val :lambda-list '(m))
(cl:defmethod state-val ((m <Bumper>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_specific_msgs-msg:state-val is deprecated.  Use robot_specific_msgs-msg:state instead.")
  (state m))
(cl:defmethod roslisp-msg-protocol:symbol-codes ((msg-type (cl:eql '<Bumper>)))
    "Constants for message type '<Bumper>"
  '((:RIGHT . 0)
    (:LEFT . 1)
    (:STATERELEASED . 0)
    (:STATEPRESSED . 1))
)
(cl:defmethod roslisp-msg-protocol:symbol-codes ((msg-type (cl:eql 'Bumper)))
    "Constants for message type 'Bumper"
  '((:RIGHT . 0)
    (:LEFT . 1)
    (:STATERELEASED . 0)
    (:STATEPRESSED . 1))
)
(cl:defmethod roslisp-msg-protocol:serialize ((msg <Bumper>) ostream)
  "Serializes a message object of type '<Bumper>"
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'bumper)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'state)) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <Bumper>) istream)
  "Deserializes a message object of type '<Bumper>"
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'bumper)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'state)) (cl:read-byte istream))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<Bumper>)))
  "Returns string type for a message object of type '<Bumper>"
  "robot_specific_msgs/Bumper")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'Bumper)))
  "Returns string type for a message object of type 'Bumper"
  "robot_specific_msgs/Bumper")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<Bumper>)))
  "Returns md5sum for a message object of type '<Bumper>"
  "4d423b2a165337e812d1b5a1cbab6b8d")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'Bumper)))
  "Returns md5sum for a message object of type 'Bumper"
  "4d423b2a165337e812d1b5a1cbab6b8d")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<Bumper>)))
  "Returns full string definition for message of type '<Bumper>"
  (cl:format cl:nil "# A message for Nao's bumpers on the feet~%~%uint8 bumper            # which bumper (left or right)~%uint8 state             # state of the bumper (pressed or released)~%~%uint8 right=0~%uint8 left=1~%~%uint8 stateReleased=0~%uint8 statePressed=1~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'Bumper)))
  "Returns full string definition for message of type 'Bumper"
  (cl:format cl:nil "# A message for Nao's bumpers on the feet~%~%uint8 bumper            # which bumper (left or right)~%uint8 state             # state of the bumper (pressed or released)~%~%uint8 right=0~%uint8 left=1~%~%uint8 stateReleased=0~%uint8 statePressed=1~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <Bumper>))
  (cl:+ 0
     1
     1
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <Bumper>))
  "Converts a ROS message object to a list"
  (cl:list 'Bumper
    (cl:cons ':bumper (bumper msg))
    (cl:cons ':state (state msg))
))
