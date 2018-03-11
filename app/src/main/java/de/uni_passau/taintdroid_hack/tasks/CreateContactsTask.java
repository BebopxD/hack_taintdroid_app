package de.uni_passau.taintdroid_hack.tasks;

import android.content.ContentResolver;
import android.os.AsyncTask;
import android.view.View;
import android.widget.ProgressBar;

import java.util.Random;

import de.uni_passau.taintdroid_hack.util.ContactsUtil;

/**
 * Background task, that creates {@link CreateContactsTask#NUM_CONTACTS_TO_CREATE} random contact
 * entries and saves them to the Contactbook.
 */
public class CreateContactsTask extends AsyncTask<String, Integer, Void> {

    private ContentResolver cr;
    private ProgressBar progressBar;

    private static final int LEN_FIRST_NAME = 5;
    private static final int LEN_SURNAME = 7;
    private static final int LEN_PHONE_NUMBER = 10;
    private static final int NUM_CONTACTS_TO_CREATE = 10;

    public CreateContactsTask(ContentResolver cr, ProgressBar progressBar) {
        this.cr = cr;
        this.progressBar = progressBar;
    }

    @Override
    protected Void doInBackground(String... strings) {
        generateContacts();
        return null;
    }

    @Override
    protected void onPreExecute() {
        super.onPreExecute();
        progressBar.setVisibility(View.VISIBLE);
    }

    @Override
    protected void onPostExecute(Void aVoid) {
        super.onPostExecute(aVoid);
        progressBar.setVisibility(View.GONE);
    }

    @Override
    protected void onProgressUpdate(Integer... progress) {
        progressBar.setProgress(progress[0]);
    }

    private void generateContacts() {
        for (int i = 0; i < NUM_CONTACTS_TO_CREATE; i++) {
            String fullName;

            StringBuilder sb = new StringBuilder();
            for (int j = 0; j < LEN_FIRST_NAME; j++) {
                sb.append(randomChar());
            }
            sb.append(' ');
            for (int j = 0; j < LEN_SURNAME; j++) {
                sb.append(randomChar());
            }
            fullName = sb.toString();
            sb.delete(0, sb.length());

            for (int j = 0; j < LEN_PHONE_NUMBER; j++) {
                sb.append(randomDigit());
            }

            ContactsUtil.writeContact(cr, fullName, sb.toString());
            publishProgress(i / NUM_CONTACTS_TO_CREATE);
        }
    }

    private static char randomChar() {
        String candidateChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        Random random = new Random();
        return candidateChars.charAt(random.nextInt(candidateChars.length()));
    }

    private static int randomDigit() {
        Random random = new Random();
        return random.nextInt(10);
    }
}
