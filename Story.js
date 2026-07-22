/* =====================================================
   STORY VIEWER ENGINE
   Instagram Style
===================================================== */

(() => {

    const storyState = {
        activeStory: null,
        timer: null,
        duration: 5000,
        progress: 0,
        startX:0
    };


    function getStories(){

        return Array.from(
            document.querySelectorAll(
                ".story-modal-backdrop:not(#create-story-modal)"
            )
        );

    }


    window.openStoryModal = function(storyId){

        if(!storyId) return;


        if(storyState.activeStory){
            closeStoryModal(
                storyState.activeStory
            );
        }


        const modal =
        document.getElementById(
            `story-modal-${storyId}`
        );


        if(!modal) return;


        storyState.activeStory = storyId;
        storyState.progress = 0;


        modal.style.display="flex";

        document.body.style.overflow="hidden";


        startProgress(storyId);

    };



    window.closeStoryModal=function(storyId){


        const modal =
        document.getElementById(
            `story-modal-${storyId}`
        );


        if(modal){
            modal.style.display="none";
        }


        clearInterval(
            storyState.timer
        );


        const progress =
        document.getElementById(
            `progress-fill-${storyId}`
        );


        if(progress){
            progress.style.width="0%";
        }


        storyState.activeStory=null;

        document.body.style.overflow="";

    };



    function startProgress(storyId){


        clearInterval(
            storyState.timer
        );


        const bar =
        document.getElementById(
            `progress-fill-${storyId}`
        );


        if(!bar) return;



        const step =
        100/(storyState.duration/50);



        storyState.timer =
        setInterval(()=>{


            storyState.progress += step;


            bar.style.width =
            storyState.progress+"%";



            if(storyState.progress>=100){


                clearInterval(
                    storyState.timer
                );


                nextStory(storyId);

            }


        },50);


    }




    function nextStory(id){


        const stories=getStories();


        const index =
        stories.findIndex(
            item =>
            item.id === `story-modal-${id}`
        );


        const next =
        stories[index+1];


        if(next){

            openStoryModal(
                next.dataset.storyId
            );

        }
        else{

            closeStoryModal(id);

        }

    }




    function previousStory(id){


        const stories=getStories();


        const index =
        stories.findIndex(
            item =>
            item.id === `story-modal-${id}`
        );


        const previous =
        stories[index-1];


        if(previous){

            openStoryModal(
                previous.dataset.storyId
            );

        }

    }




    window.handleStoryViewClick =
    function(event,id){


        if(
            event.target.closest("button")
            ||
            event.target.closest("form")
        ){
            return;
        }



        const card =
        event.currentTarget
        .querySelector(".story-view-card");



        if(!card)return;



        const position =
        event.clientX -
        card.getBoundingClientRect().left;



        if(position <
            card.offsetWidth*0.3
        ){

            previousStory(id);

        }
        else{

            nextStory(id);

        }

    };




    document.addEventListener(
        "DOMContentLoaded",
        ()=>{


        document
        .querySelectorAll(
            ".story-full-image"
        )
        .forEach(img=>{

            img.draggable=false;

        });



        document.addEventListener(
            "keydown",
            e=>{

                if(e.key==="Escape"
                &&
                storyState.activeStory){

                    closeStoryModal(
                        storyState.activeStory
                    );

                }

            }
        );



        document.addEventListener(
            "click",
            e=>{


                if(
                    e.target.classList.contains(
                        "story-modal-backdrop"
                    )
                ){

                    closeStoryModal(
                        storyState.activeStory
                    );

                }


            }
        );



        document.addEventListener(
            "touchstart",
            e=>{


                if(!storyState.activeStory)
                    return;


                storyState.startX =
                e.changedTouches[0].screenX;


            },
            {passive:true}
        );



        document.addEventListener(
            "touchend",
            e=>{


                if(!storyState.activeStory)
                    return;


                let end =
                e.changedTouches[0].screenX;


                let diff =
                storyState.startX-end;



                if(Math.abs(diff)>60){


                    if(diff>0){

                        nextStory(
                            storyState.activeStory
                        );

                    }
                    else{

                        previousStory(
                            storyState.activeStory
                        );

                    }

                }


            },
            {passive:true}
        );


    });


})();
