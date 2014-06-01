//Written by Dad on 2/16 to work out how to create an index that grows with the number of segments
//and gives an address in the Measurement array for the location to store the distance 
//measurement.

const byte span=180; //how wide is the sweep in degrees
const byte segments=6; //how many segments will the span be divided into
const byte observationPoints = segments+1; //finds how many observation points there are

const byte arrayColumns = observationPoints - 4; //finds the array width to hold measurement data

void setup() {
  
  // initialize serial communication:
  Serial.begin(9600);
   
   //Set up the matrix to store sensor measurement data with zeros in each element AND
   //set up the index of how to locate each array location.
   int measurementArray[3][arrayColumns];
   int measurementIndex[observationPoints][3];
   byte pointer=0;//pointer for the measurementIndex
   
   Serial.println("I'm going to walk through the array");
 
   for (int j=arrayColumns-1; j>=0; j--) {
      for (int i=2; i>=0; i--) { 
         measurementArray[i][j]=0;
         
         //test to see if the Array address belongs in the index
         //recall that we only use the right, top and left edge of the batrix   
         
         //right edge
         if (j==arrayColumns-1) {
             measurementIndex[pointer][0]=i;
             measurementIndex[pointer][1]=j;
             pointer++;
         }
         //top row
         else if (i==0){
             measurementIndex[pointer][0]=i;
             measurementIndex[pointer][1]=j;
             pointer++;
         }
         //left edge
         else if (j==0){
             measurementIndex[pointer][0]=i;
             measurementIndex[pointer][1]=j;
             pointer++;
         }
         
         // This section prints the array for testing.
         Serial.print("Element [");
         Serial.print(i);
         Serial.print("][");
         Serial.print(j);
         Serial.print("] is: ");
         Serial.println(measurementArray[i][j]);
      } 
   }
   
   //walk through the measurementIndex
   Serial.println("Now I'm walking through the index");
   for(int i=0; i<observationPoints; i++){
           Serial.print("Index [");
           Serial.print(i);
           Serial.print("] is: ");
           
           for(int j=0; j<2; j++) {
             //Serial.print("[");
             Serial.print(measurementIndex[i][j]);
             Serial.print(" ");
           }
           
           Serial.println();
   }
    
   //When you view the last bit of code it has the last three values in the index in the wrong order
   //this next bit swaps the last and 2nd to last elements to get them right.
   int lastElement = observationPoints-1;
   int secondToLastElement = observationPoints-3;
   int tempArray1[] = {measurementIndex[lastElement][0], measurementIndex[lastElement][1]}; 
   int tempArray2[] = {measurementIndex[secondToLastElement][0], measurementIndex[secondToLastElement][1]};
   //Serial.println(tempArray2[0]);
   //Serial.println(tempArray2[1]);
   //swap the last element
   measurementIndex[lastElement][0]=tempArray2[0];
   measurementIndex[lastElement][1]=tempArray2[1];
   //swap the 2nd to last element
   measurementIndex[secondToLastElement][0]=tempArray1[0];
   measurementIndex[secondToLastElement][1]=tempArray1[1];
   
   //now lets take another run through the measurement Index
   Serial.println("Let's walk through the index again");
   for(int i=0; i<observationPoints; i++){
           Serial.print("Index [");
           Serial.print(i);
           Serial.print("] is: ");
           
           for(int j=0; j<2; j++) {
             //Serial.print("[");
             Serial.print(measurementIndex[i][j]);
             Serial.print(" ");
           }
           
           Serial.println();
   }  
   
}

void loop() {
  delay(10000);
  Serial.println("Now i'm in the loop.");
}
