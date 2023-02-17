package com.example.maji_mkulima;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

public class Signup extends AppCompatActivity {

    // create object of DatabaseReference class to access firebase's realtime Database
    DatabaseReference databaseReference = FirebaseDatabase.getInstance().getReferenceFromUrl("https://maji-mkulima-base-default-rtdb.europe-west1.firebasedatabase.app/");


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_signup);

        final EditText fullname = findViewById(R.id.fullname);
        final EditText email = findViewById(R.id.email);
        final EditText phone = findViewById(R.id.phone);
        final EditText password = findViewById(R.id.password);
        final EditText conpassword = findViewById(R.id.conpassword);

        final Button signBtn = findViewById(R.id.signBtnBtn);
        final TextView loginBtn = findViewById(R.id.loginBtn);

        signBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                // get data from EditTexts into String variables
                final String fullnameTxt = fullname.getText().toString();
                final String emailTxt = email.getText().toString();
                final String phoneTxt = phone.getText().toString();
                final String passwordTxt = password.getText().toString();
                final String conPassTxt = conpassword.getText().toString();

                // check if the user filled all the fields before sending data to firebase
                if(fullnameTxt.isEmpty() || emailTxt.isEmpty() || phoneTxt.isEmpty() || passwordTxt.isEmpty()){
                    Toast.makeText(Signup.this, "Please fill in all the fields", Toast.LENGTH_SHORT).show();

                }

                //check if passwords are matching
                // if not, show a toast message
                else if(!passwordTxt.equals(conPassTxt)){
                    Toast.makeText(Signup.this, "Passwords did not match", Toast.LENGTH_SHORT).show();
                }

                else{

                    databaseReference.child("Users").addListenerForSingleValueEvent(new ValueEventListener() {
                        @Override
                        public void onDataChange(@NonNull DataSnapshot snapshot) {

                            //check if phone is not registered before
                            if (snapshot.hasChild(phoneTxt)){
                                Toast.makeText(Signup.this, "Phone already registered", Toast.LENGTH_SHORT).show();
                            }
                            else{
                                //sending data to firebase realtime database
                                //we are using phone number as unique user identifier
                                //so all the other details of user comes under phone number
                                databaseReference.child("Users").child(phoneTxt).child("Fullname").setValue(fullnameTxt);
                                databaseReference.child("Users").child(phoneTxt).child("Email").setValue(emailTxt);
                                databaseReference.child("Users").child(phoneTxt).child("Password").setValue(passwordTxt);

                                //show success message then finish the activity
                                Toast.makeText(Signup.this, "Sign-up Successful", Toast.LENGTH_SHORT).show();
                                finish();
                            }
                        }

                        @Override
                        public void onCancelled(@NonNull DatabaseError error) {

                        }
                    });


                }

            }
        });
        loginBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
            }
        });

    }


}