/* ================= STORY VIEWER CLICK SUPPORT ================= */

document.querySelectorAll(".story-item").forEach(item => {

    item.addEventListener("click", function(e){

        if(this.id === "open-story-popup"){
            return;
        }

        const storyId = this.dataset.storyId;

        if(storyId){
            e.preventDefault();
            openStoryModal(storyId);
        }

    });

});
