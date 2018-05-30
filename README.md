[![Build Status](https://travis-ci.org/statonlab/tripal_ssr.svg?branch=master)](https://travis-ci.org/statonlab/tripal_ssr)


This module displays **SSR (simple sequence repeat)** data.  It assumes you have identified SSRs in your data and would like to annotate your features with these SSRs.  

The Tripal SSR module is not considered a complete module: this is why the version tag is `7.x-0.1-pre-pre-pre-alpha`.


## Loading data

SSR data is loaded using the `tripal_bulk_loader` module.  When the module is installed, a custom data loader should be added to the bulk loader admin panel (Admin->Tripal->Data_Loaders->Bulk_Loader).  Click **Add Bulk Loading Job** to ue the loader template.  The template can be viewed by clicking on the Templates tab. 

The bulk loader expects a  tab-delimited file on the server with the following 9 columns:

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

## Materialized view
The `tripal_ssr` module utilizes a materialized view table.  Once your data is loaded, you must populate the view.  Tripal->Data_Storage->Materialized_Views, click to populate the tripal_ssr mview, and run the job.

## Custom view

A custom view should be manually created by the site administrator to list all SSRs assocaited with a given organism, and to provide a link to all SSRs in `fasta` format. 

More documentation will be added soon.


## Chado tables

`tripal_ssr` make use of two Chado table: `featureprop` and `tripal_ssr_mview`.  The bulk loader adds columns 2-9 of the above input file into `featureprop` for the specified feature.  The `tripal_ssr_mview` table, when populated, tracks which features were annotated with SSRs for each organism.


## Tripal 3
#### Features
The current master branch is compatible with Tripal 3.  `featureprops` are automatically migrated as fields in the `tripal` ontology (ie. tripal__tripal_ssr_motif): to ensure they appear on your feature page, go to Structure->Tripal Content Type -> [feature of interest: ie, Gene], and click + Check for New Field.  The terms should appear, and can be organized within their own pane by clicking on **Manage Display**.
#### Organisms
A new Field, `so__microsatellite`, is introduced for the Organism bundle.  Navigate to Structure->Tripal Content Type -> Organism, and click + Check for New Field.  The organism field instance provide a count of the SSRs for that organism, and a link to the SSR view for that organism providing a list of all feature and a bulk download option.