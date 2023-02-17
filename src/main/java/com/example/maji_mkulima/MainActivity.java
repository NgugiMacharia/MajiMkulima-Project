package com.example.maji_mkulima;

import static com.google.firebase.database.FirebaseDatabase.*;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Toast;

import com.google.android.gms.tasks.Task;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {

    private Button open;
    private EditText name;
    private Button refresh;
    private ListView view;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        open = findViewById(R.id.open);
        name = findViewById(R.id.name);
        refresh = findViewById(R.id.refresh);
        view = findViewById(R.id.view);

        /*refresh.setOnClickListener({
                Toast.makeText(MainActivity.this,
                        "Refreshed",
                        Toast.LENGTH_SHORT).show();
                startActivity(new Intent(MainActivity.this,
                        MainActivity.class));
        });

        open.setOnClickListener({
                FirebaseDatabase.getInstance().getReference().child("DataID").push().child("Valve Status").setValue("Open");
        });
*/
        ArrayList<String> list = new ArrayList<>();
        ArrayAdapter<String> adapter = new ArrayAdapter<>(this, R.layout.list_item, list);
        view.setAdapter(adapter);

        DatabaseReference reference = getInstance().getReference().child("DataID");
        reference.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot dataSnapshot) {
                list.clear();
                for (DataSnapshot snapshot1 : dataSnapshot.getChildren()){
                    list.add((String) snapshot1.getValue());
                }
                adapter.notifyDataSetChanged();
            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {

            }
        });

    }
}