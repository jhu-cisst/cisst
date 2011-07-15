/* -*- Mode: Java; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=java softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Martin Kelly, Anton Deguet
  Created on: 2011-02-15

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

import edu.cmu.sphinx.decoder.search.Token;
import edu.cmu.sphinx.frontend.util.Microphone;
import edu.cmu.sphinx.recognizer.Recognizer;
import edu.cmu.sphinx.result.*;
import edu.cmu.sphinx.util.props.ConfigurationManager;

import java.text.DecimalFormat;
import java.util.HashMap;
import javax.sound.sampled.*;

class cisstSphinx4 extends Thread {
    private HashMap<String, Recognizer> SphinxRecognizers;
    private Recognizer CurrentSphinxRecognizer;
    private ConfigurationManager SphinxConfigurationManager;
    private Microphone Microphone; 
    private boolean RecognitionPaused; 

    // static method
    private native void WordRecognizedCallback(long sphinx4WrapperPointer, String word);
    // library that defines static method called via JNI
    static {
        System.loadLibrary("cisstSphinx4");
    }
    // pointer to C++ object used by WordRecognized
    private long Sphinx4WrapperPointer;

    // sets the active context
    public void SetCurrentContext(String context) {
        if (SphinxRecognizers.containsKey(context)) {
            CurrentSphinxRecognizer = SphinxRecognizers.get(context);
        } else {
            throw new RuntimeException("Java Sphinx wrapper: could not switch to context \"" + context + "\"");
        }
        PauseRecognition();
    }

    public void PrintAudioDevices() {
        System.out.println("Java Sphinx wrapper: audio devices installed on your system:");
        Mixer.Info[] mixerInfo = AudioSystem.getMixerInfo();
        for (int i = 0; i < mixerInfo.length; i++) {
            Mixer mixer = AudioSystem.getMixer(mixerInfo[i]);
            System.out.println(" - Mixer[" + i + "]: " + mixerInfo[i].getName());
            System.out.println("   Description: " + mixerInfo[i].getDescription());
        }
    }

    public void Start(long sphinx4WrapperPointer,
                      String configName,
                      String[] contextList,
                      String startingContext) {
        System.out.println("Java Sphinx wrapper: start");
        Sphinx4WrapperPointer = sphinx4WrapperPointer;
        PrintAudioDevices(); // to be removed
        SphinxRecognizers = new HashMap<String, Recognizer>();

        System.out.println("Java sphinx wrapper: create configuration manager");
        try {
            SphinxConfigurationManager = new ConfigurationManager(cisstSphinx4.class.getResource(configName));
        } catch (Exception e) {
            System.out.println("Java sphinx wrapper: exception while creating Sphinx configuration manager for file:");
            System.out.println(configName);
            e.printStackTrace();
            System.exit(1);
        }

        System.out.println("Java sphinx wrapper: create list of Sphinx recognizers corresponding to contexts");
        // make map from context name to recognizer instance
        try {
            for (String context : contextList) {
                Recognizer contextRecognizer = (Recognizer) SphinxConfigurationManager.lookup(context + "Recognizer");
                contextRecognizer.allocate();
                SphinxRecognizers.put(context, contextRecognizer);
            }
        } catch (Exception e) {
            System.out.println("Java sphinx wrapper: exception while creating contexts");
            e.printStackTrace();
            System.exit(1);
        }

        System.out.println("Java sphinx wrapper: set current context");
        // must set it to null first or the Java compiler will complain... *sigh*
        CurrentSphinxRecognizer = null;
        SetCurrentContext(startingContext);
        System.out.println("Java sphinx wrapper: looking for microphone");
        // start the microphone or exit if the program if this is not possible
        Microphone = (Microphone) SphinxConfigurationManager.lookup("microphone");
        System.out.println("Java sphinx wrapper: start recording");
        try {
            Microphone.startRecording();
        } catch (Exception e) {
            System.out.println("Java sphinx wrapper: cannot start microphone");
            e.printStackTrace();
            PrintAudioDevices();
            System.exit(1);
        }
        System.out.print("Java sphinx wrapper: microphone is setup: ");
        System.out.println(Microphone.getName());

        RecognitionPaused = false;

        // start separate recognition thread
        start();
    }

    public void PauseRecognition() {
        if (Microphone != null) {
            Microphone.stopRecording();
            RecognitionPaused = true;
        }
    }

    public void run() {
        while (true) {
            if (RecognitionPaused) {
                Microphone.startRecording();
                RecognitionPaused = false;
            }
            Result result = CurrentSphinxRecognizer.recognize();
            String word;
            if (result != null) {
                word = result.getBestFinalResultNoFiller();
                // System.out.println("-----");
    //             System.out.println(word);

    //             try {
    //                 DecimalFormat format = new DecimalFormat("#.#####");
    //                 ConfidenceScorer cs = (ConfidenceScorer) SphinxConfigurationManager.lookup("confidenceScorer");
    //                 ConfidenceResult cr = cs.score(result);
    //                 Path best = cr.getBestHypothesis();

    //                 System.out.println(best.getTranscription());
    //                 System.out.println("(confidence: " + format.format(best.getLogMath().logToLinear((float) best.getConfidence())) + ')');
    //                 System.out.println();
    //                 WordResult[] words = best.getWords();
    //                 for (WordResult wr : words) {
    //                     printWordConfidence(wr);
    //                 }
    //                 System.out.println();
    //             } catch (Exception e) {
    //                 System.out.println("Java: exception while scoring");
    //                 e.printStackTrace();
    //             }

            } else {
                word = "";
            }
            // finally, send word to C++ side
            WordRecognizedCallback(Sphinx4WrapperPointer, word);
        }
    }

    private static void printWordConfidence(WordResult wr) {
        DecimalFormat format = new DecimalFormat("#.#####");
        String word = wr.getPronunciation().getWord().getSpelling();
        System.out.print(word);
        System.out.println(" (confidence: " + format.format(wr.getLogMath().logToLinear((float) wr.getConfidence())) + ')');
    }
}
