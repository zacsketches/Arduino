//test of strcmp for Alfred2
struct Control_vec{
	char label[13];
        int m1_dir;
	int m1_pwm;
	int m2_dir;
	int m2_pwm;
	bool overwrite;		//true if the control_vec is to completely overwrite the existing vector.
};

const Control_vec forward = {"forward", 0, 150, 0, 150, true};
const Control_vec backward = {"backward", 0, 100, 1, 100, true};
const Control_vec ease_right = {"ease_right", 0, 150, 0, 100, true};
const Control_vec ease_left = {"ease_left", 0, 100, 0, 150, true};
const Control_vec swerve_right = {"swerve_right", 0, 200, 0, 100, true};
const Control_vec swerve_left = {"swerve_left", 0, 100, 0, 200, true};
const Control_vec twist_right = {"twist_right", 0, 100, 1, 100, true};
const int number_of_controls = 7;
Control_vec controls[number_of_controls];

void setup() {
  Serial.begin(9600);
  
  //push the control vectors into the array
  controls[0] = forward;
  controls[1] = backward;
  controls[2] = ease_right;
  controls[3] = ease_left;
  controls[4] = swerve_right;
  controls[5] = swerve_left;
  controls[6] = twist_right;
    
}

void loop() {
  int z = find_control_index("ease_left");
  Serial.print("z is equal to: ");
  Serial.println(z);
  delay(1000);
  

}

int find_control_index(const char* target) {
  int res;
  for (int i=0; i!=number_of_controls; i++) {
     const char* control_label = controls[i].label;
     if(!strcmp(control_label, target)) {
       res = i;
     }
  }
  return res;
}
