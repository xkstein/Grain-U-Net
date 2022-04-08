# Aug 22 

*Changes:* I removed the dropout stages in the model
*Info:* Trained for 3 epochs, 2000 steps per epoch

| Image | Mean IOU |
| --- | --- |
| img 0 | Mean IOU: 0.6897960320932874 |
| img 1 | Mean IOU: 0.6918237807413546 |
| img 10 | Mean IOU: 0.32937398306008037 |
| img 11 | Mean IOU: 0.4090040275747048  |
| img 12 | Mean IOU: 0.3772879304429595  |
| img 13 | Mean IOU: 0.2836940215242354  |
| img 14 | Mean IOU: 0.263733829255234   |
| img 15 | Mean IOU: 0.3609622702541213  |
| img 16 | Mean IOU: 0.3174240846292632  |
| img 17 | Mean IOU: 0.13929159806323452 |
| img 18 | Mean IOU: 0.29501815187293223 |
| img 19 | Mean IOU: 0.3441311127996548  |
| img 2  | Mean IOU: 0.5405584809756075  |
| img 20 | Mean IOU: 0.3092728479491392  |
| img 21 | Mean IOU: 0.0923450719252461  |
| img 22 | Mean IOU: 0.35390273997218114 |
| img 23 | Mean IOU: 0.32514641024161567 |
| img 24 | Mean IOU: 0.3260055477566883  |
| img 25 | Mean IOU: 0.3415242557556229  |
| img 26 | Mean IOU: 0.2908972545854925  |
| img 27 | Mean IOU: 0.1339033245176248  |
| img 28 | Mean IOU: 0.19717508171493314 |
| img 3  | Mean IOU: 0.4149391995491761  |
| img 4  | Mean IOU: 0.3199635288446575  |
| img 5  | Mean IOU: 0.4426870778501892  |
| img 6  | Mean IOU: 0.2228128581305763  |
| img 7  | Mean IOU: 0.19792779714155598 |
| img 8  | Mean IOU: 0.4158658691258742  |
| img 9  | Mean IOU: 0.36183258993041406 |

Mean Mean IOU = 0.33752761235440193
Median Mean IOU = 0.3260055477566883


Max Mean IOU = 0.6918237807413546, Min Mean IOU = 0.0923450719252461

---

# 2022 March 09

**Changes:** 

- We completely changed the training set
  - now we're in "train_nouveaux_256" era (with 256 indicating that the images are all pre-downscaled to 256x256)
- Also the data importer has be changed (correcting some old errors)

*Info:* Trained for 66 epochs, batch size 5 with training set of 269 images (53 steps) and with simple augmentation

| Image Name | Mean IOU |
| --- | --- |
| data/test_combination/4HR/2414 | 0.7000489659491878 |
| data/test_combination/4HR/2465 | 0.6581298410056843 |
| data/test_combination/4HR/2436 | 0.7630426240382149 |
| data/test_combination/4HR/2507 | 0.6948745865435432 |
| data/test_combination/4HR/2495 | 0.8061037449540067 |
| data/test_combination/4HR/2434 | 0.6539124882548515 |
| data/test_combination/10HR/2136 | 0.7149708871979714 |
| data/test_combination/10HR/2153 | 0.7452933525560553 |
| data/test_combination/1HR/2753 | 0.693630512685243 |
| data/test_combination/1HR/2741 | 0.5640754497849219 |
| data/test_combination/1HR/2789 | 0.624683278661852 |
| data/test_combination/2HR/2364 | 0.6259406162152344 |
| data/test_combination/2HR/2349 | 0.6027762177578669 |


Mean Mean IOU = 0.6805755819695872
Median Mean IOU = 0.693630512685243


---
# 2022 March 09

> Ran on pruned images

| Image Name | Mean IOU |
| --- | --- |
| data/test_all/4HR/2413 | 0.5370098138535687 |
| data/test_all/4HR/2414 | 0.7488207481376464 |
| data/test_all/4HR/2465 | 0.633557185294331 |
| data/test_all/4HR/2436 | 0.7499657513588235 |
| data/test_all/4HR/2507 | 0.6880448353317697 |
| data/test_all/4HR/2495 | 0.7921568124372045 |
| data/test_all/4HR/2434 | 0.7087634400557158 |
| data/test_all/10HR/2159 | 0.770482723838771 |
| data/test_all/10HR/2144 | 0.7946577201113371 |
| data/test_all/10HR/2127 | 0.8173588216697174 |
| data/test_all/10HR/2400 | 0.8153237942412302 |
| data/test_all/10HR/2429 | 0.8037462086421405 |
| data/test_all/10HR/2155 | 0.7211639986679867 |
| data/test_all/10HR/2136 | 0.7011175095238514 |
| data/test_all/10HR/2153 | 0.7521818642725284 |
| data/test_all/10HR/2405 | 0.7789907745333283 |
| data/test_all/1HR/2753 | 0.7229935839443175 |
| data/test_all/1HR/2741 | 0.603861958830912 |
| data/test_all/1HR/2761 | 0.46576499420882606 |
| data/test_all/1HR/2789 | 0.6254746852600008 |
| data/test_all/1HR/2745 | 0.53127672342365 |
| data/test_all/2HR/2364 | 0.62606767015159 |
| data/test_all/2HR/2354 | 0.6099694189340689 |
| data/test_all/2HR/2331 | 0.5654622027790536 |
| data/test_all/2HR/2314 | 0.6505530360321126 |
| data/test_all/2HR/2349 | 0.5804595375560594 |
| data/test_all/2HR/2312 | 0.6349992839206333 |
| data/test_all/2HR/2356 | 0.2695248933012071 |
| data/test_all/2HR/2317 | 0.5145794474331997 |


Mean Mean IOU = 0.6625630840601924
Median Mean IOU = 0.6880448353317697


---
# 2022 March 09

> Ran on un-pruned images

| Image Name | Mean IOU |
| --- | --- |
| data/test_all/4HR/2413 | 0.5368351514821104 |
| data/test_all/4HR/2414 | 0.7478899604221069 |
| data/test_all/4HR/2465 | 0.6331807521909644 |
| data/test_all/4HR/2436 | 0.7493831404177137 |
| data/test_all/4HR/2507 | 0.6877127196878631 |
| data/test_all/4HR/2495 | 0.7912839421576664 |
| data/test_all/4HR/2434 | 0.707598830669797 |
| data/test_all/10HR/2159 | 0.7694465882364534 |
| data/test_all/10HR/2144 | 0.7941328748340971 |
| data/test_all/10HR/2127 | 0.8167809019806195 |
| data/test_all/10HR/2400 | 0.814690529881481 |
| data/test_all/10HR/2429 | 0.8034437715504268 |
| data/test_all/10HR/2155 | 0.732774296008579 |
| data/test_all/10HR/2136 | 0.7006639970797585 |
| data/test_all/10HR/2153 | 0.7519133108834194 |
| data/test_all/10HR/2405 | 0.7772285749134218 |
| data/test_all/1HR/2753 | 0.7138246898268857 |
| data/test_all/1HR/2741 | 0.6033609068764592 |
| data/test_all/1HR/2761 | 0.4653864054466636 |
| data/test_all/1HR/2789 | 0.624683278661852 |
| data/test_all/1HR/2745 | 0.5309909989705623 |
| data/test_all/2HR/2364 | 0.6259406162152344 |
| data/test_all/2HR/2354 | 0.6092451947491839 |
| data/test_all/2HR/2331 | 0.5650914627834408 |
| data/test_all/2HR/2314 | 0.6504658332695303 |
| data/test_all/2HR/2349 | 0.5800597943270815 |
| data/test_all/2HR/2312 | 0.634665801163692 |
| data/test_all/2HR/2356 | 0.2694575187381867 |
| data/test_all/2HR/2317 | 0.5234065610058973 |


Mean Mean IOU = 0.6624668415321084
Median Mean IOU = 0.6877127196878631

