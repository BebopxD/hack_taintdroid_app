package de.uni_passau.taintdroid_hack.util;

import android.content.ContentProviderOperation;
import android.content.ContentResolver;
import android.content.OperationApplicationException;
import android.database.Cursor;
import android.os.RemoteException;
import android.support.annotation.NonNull;
import android.util.Log;

import java.util.ArrayList;

import de.uni_passau.taintdroid_hack.Constants;

import static android.provider.ContactsContract.AUTHORITY;
import static android.provider.ContactsContract.CommonDataKinds.Phone;
import static android.provider.ContactsContract.CommonDataKinds.StructuredName;
import static android.provider.ContactsContract.Contacts;
import static android.provider.ContactsContract.Data;
import static android.provider.ContactsContract.RawContacts;

/**
 * Helperclass to access contact information on an android phone.
 */
public class ContactsUtil {

    /**
     *
     *
     * @param cr
     * @param displayName
     * @param number
     */
    public static void writeContact(ContentResolver cr, String displayName, String number) {
        ArrayList<ContentProviderOperation> contentProviderOperations = new ArrayList<>();
        //insert raw contact using RawContacts.CONTENT_URI
        contentProviderOperations.add(ContentProviderOperation.newInsert(RawContacts.CONTENT_URI)
                .withValue(RawContacts.ACCOUNT_TYPE, null)
                .withValue(RawContacts.ACCOUNT_NAME, null).build());
        //insert contact display name using Data.CONTENT_URI
        contentProviderOperations.add(ContentProviderOperation.newInsert(Data.CONTENT_URI)
                .withValueBackReference(Data.RAW_CONTACT_ID, 0)
                .withValue(Data.MIMETYPE, StructuredName.CONTENT_ITEM_TYPE)
                .withValue(StructuredName.DISPLAY_NAME, displayName).build());
        //insert mobile number using Data.CONTENT_URI
        contentProviderOperations.add(ContentProviderOperation.newInsert(Data.CONTENT_URI)
                .withValueBackReference(Data.RAW_CONTACT_ID, 0)
                .withValue(Data.MIMETYPE, Phone.CONTENT_ITEM_TYPE)
                .withValue(Phone.NUMBER, number)
                .withValue(Phone.TYPE, Phone.TYPE_MOBILE).build());
        try {
            cr.applyBatch(AUTHORITY, contentProviderOperations);
        } catch (RemoteException e) {
            Log.e(Constants.TAG, "Got exception while writing contacts to phone book", e);
        } catch (OperationApplicationException e) {
            Log.e(Constants.TAG, "Got exception while writing contacts to phone book", e);
        }
    }

    @NonNull
    public static String getContactsAsString(@NonNull ContentResolver cr) {
        Cursor query = cr.query(Contacts.CONTENT_URI, null, null, null, null);
        final StringBuilder sb = new StringBuilder();
        try {
            if ((query != null ? query.getCount() : 0) > 0) {
                while (query != null && query.moveToNext()) {
                    String id = query.getString(
                            query.getColumnIndex(Contacts._ID));
                    String name = query.getString(query.getColumnIndex(
                            Contacts.DISPLAY_NAME));

                    if (query.getInt(query.getColumnIndex(
                            Contacts.HAS_PHONE_NUMBER)) > 0) {
                        Cursor pquery = cr.query(Phone.CONTENT_URI, null,
                                Phone.CONTACT_ID + " = ?", new String[]{id}, null);
                        while (pquery != null && pquery.moveToNext()) {
                            String phoneNo = pquery.getString(pquery.getColumnIndex(
                                    Phone.NUMBER));
                            sb.append("Name: ").append(name).append("\n")
                                    .append("Phone Number: ").append(phoneNo).append("\n");
                        }
                        if (pquery != null) pquery.close();
                    }
                }
            }
        } finally {
            if (query != null) query.close();
        }
        return sb.toString();
    }
}
