Building Launchpad Recipes
==========================
- ## Log in to launchpad using your ubuntu one login  
- ## Select "Register a Project" at [Launchpad's Home Page](http://launchpad.net)  

![get_started](recipe-images/recipe_image_1.png) 

- ## In the next screen enter the required information into the form and click continue  

![register_new_project](recipe-images/recipe_image_2.png) 
  
- ## Make sure this is not a duplicate project  

![check_for_duplicates](recipe-images/recipe_image_3.png) 

- ## Enter the Registration Detailsand clock on Complete Registration

![registration_details](recipe-images/recipe_image_4.png) 

- ## On the right hand side of your screen you will see a box like this, select code

![set_up_code](recipe-images/recipe_image_5.png) 

- ## On this screen you can enter the git repository url that you will build the recipe from  

![configure_code](recipe-images/recipe_image_6.png) 

- ## From this screen select "Create Packaging Recipe" Note-> it may take a while for the initial import, be patient

![create_recipe](recipe-images/recipe_image_7.png) 

- ## The next screen will allow you to enter the details of your recipe. Create a new PPA for the results and choose what distributions you want to build for.  Xenial, Bionic and Eoan are all that are in support right now. The default build of the master branch is all you need in the recipe text

![import_details](recipe-images/recipe_image_8.png) 

- ## On this next screen you can manually build the recipe by selecting "Build Now" or "Request Build".

![build_now](recipe-images/recipe_image_9.png)

- ## Note, if a build has already begun you will not be able to build for those distributions.

![in_progress](recipe-images/recipe_image_10.png) 

- ## you will be able to watch the status of your build as it progresses.

![pending_build](recipe-images/recipe_image_11.png) 


- ## Once the build has finished, add your new PPA to your system as usual and install the package(s)