[![Build Status](https://travis-ci.org/statonlab/tripal_ssr.svg?branch=master)](https://travis-ci.org/statonlab/tripal_ssr)


This module displays **SSR (simple sequence repeat)** data.  It assumes you have identified SSRs in your data and would like to annotate your features with these SSRs.  

## Generating data

[SSRs can be predicted using these script](https://github.com/mestato/lab_code/tree/master/hwg_gssr_scripts).  Alternatively SSRs can be experimentally determined and put into a spreadsheet that meets the criteria below.

## Loading data

SSR data is loaded using a TripalImporter.  

The Importer expects a  tab-delimited file on the server with the following 9 columns:

|  column |description   | 
|---|---|
|feature   | Feature name that contain the SSR  | 
| motif  | The repeat motif or base pattern that i repeated.  For example, AT  | 
| repeats  | How many times the motif is repeated  |
| start  | Start location in the parent feature  |
| end  | End location in the parent feature  |
| fprimer  | A forward primer to amplify the SSR  |
| rprimer  | A reverse primer to amplify the SSR  |
| ftm  | The forward primer's calculated Tm  |
| rtm  |  The reverse primer's calculated Tm |
| size  | The expected size of the PCR product  |


## Tripal 3
#### Features
The current master branch is compatible with Tripal 3.  `featureprops` are automatically migrated as fields in the `tripal` ontology (ie. tripal__tripal_ssr_motif): to ensure they appear on your feature page, go to Structure->Tripal Content Type -> [feature of interest: ie, Gene], and click + Check for New Field.  The terms should appear, and can be organized within their own pane by clicking on **Manage Display**.
#### Organisms
A new Field, `so__microsatellite`, is introduced for the Organism bundle.  Navigate to Structure->Tripal Content Type -> Organism, and click + Check for New Field.  The organism field instance provide a count of the SSRs for that organism, and a link to the SSR view for that organism providing a list of all feature and a bulk download option.