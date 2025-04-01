package com.example.insidethescore.auxiliary;

import android.util.Log;

public class SearchEngine {
    String searchToken;  // user typed text for search

    String lowerLimitToken;  // lower limit used in search. Search will return tokes that are alphabetically bigger than lowerLimit.
    String upperLimitToken;  // upper limit used in search. Search will return tokes that are alphabetically smaller than upperLimit.

    public SearchEngine() {
        searchToken = "";
        lowerLimitToken = "";
        upperLimitToken = "";
    }

    /**
     * Extract lowerLimit and upperLimit for searchToken.
     * @param searchTok the token that's going to be searched.
     */
    public void generateSearchLimits(String searchTok) {
        searchToken = canonicalizeToken(searchTok);
        lowerLimitToken = searchToken;

        upperLimitToken = extractUpperLimitTokenLowerCase(searchTok.toLowerCase(), ' ');
        upperLimitToken = canonicalizeToken(upperLimitToken);

        Log.d("generateSearchLimits", searchToken + "\n" + lowerLimitToken + "\n" + upperLimitToken);
    }

    /**
     * Transform string token in a cononical form (camel case -first word's first letter capital also- in this case).
     * @param token token to be transformed
     * @return the transformed token
     */
    public String canonicalizeToken(String token) {
        String canonicalizedToken = toCamelCase(token, ' ');
        Log.d("canonicalizeToken", "CamelCase: " + canonicalizedToken);

        return canonicalizedToken;
    }

    /**
     * Extract upperLimit for token. upperLimit is the token, with the rightmost character
     * changed to the alphabetically next character. If rightmost character is 'z', then
     * we go to the character on the left, until some character is not a 'z'
     * (e.g. for token "bee", we get "bef", for token "bez", we get "bfa").
     * @param token
     * @param delimiter
     * @return
     */
    private String extractUpperLimitTokenLowerCase(String token, char delimiter) {
        int tokLength = token.length();
        final char lastLetter = 'z';

        StringBuilder builder = new StringBuilder(token);

        for (int i = tokLength-1; i>= 0; i--) {
            char thisLetter = token.charAt(i);

            if (thisLetter != delimiter) {
                if (thisLetter < lastLetter) {
                    char nextChar = thisLetter;
                    nextChar++;

                    builder.setCharAt(i, nextChar);

                    return builder.toString();
                } else {
                    builder.setCharAt(i, 'a');
                }
            }
        }

        // special case for all 'z' token form (e.g. "zzzz z zzz zzzz")
        // upperLimitToken must be the same with an extra 'a' in the end
        // (for this example, "zzzz z zzz zzzzb")
        for (int i = 0; i < tokLength; i++) {
            if (builder.toString().charAt(i) != delimiter) {
                builder.setCharAt(i, lastLetter);
            }
        }

        return builder.toString().concat("b");
    }

    /**
     * Taken from "https://www.baeldung.com/java-string-to-camel-case"
     * @param text text to transform to camel case.
     * @param delimiter the delimiter that separates words.
     * @return the transformed text
     */
    public String toCamelCase(String text, char delimiter) {
        boolean shouldConvertNextCharToLower = false;

        StringBuilder builder = new StringBuilder();

        for (int i = 0; i < text.length(); i++) {
            char currentChar = text.charAt(i);
            if (currentChar == delimiter) {
                shouldConvertNextCharToLower = false;
                builder.append(currentChar);
            } else if (shouldConvertNextCharToLower) {
                builder.append(Character.toLowerCase(currentChar));
            } else {
                builder.append(Character.toUpperCase(currentChar));
                shouldConvertNextCharToLower = true;
            }
        }
        return builder.toString();
    }

    public String getLowerLimitToken() {
        return lowerLimitToken;
    }

    public String getUpperLimitToken() {
        return upperLimitToken;
    }
}
