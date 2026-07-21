
document.addEventListener("DOMContentLoaded", function () {

   
    const csrfToken = document.querySelector("[name=csrfmiddlewaretoken]")?.value || 
                      (typeof csrftoken !== "undefined" ? csrftoken : "");

    
    const imageInput = document.getElementById("image-input");
    if (imageInput) {
        imageInput.addEventListener("change", function () {
            const previewBox = document.getElementById("image-preview-box");
            const previewImage = document.getElementById("image-preview");
            
            if (this.files && this.files[0]) {
                const reader = new FileReader();
                reader.onload = function (e) {
                    previewImage.src = e.target.result;
                    if (previewBox) previewBox.style.display = "block";
                };
                reader.readAsDataURL(this.files[0]);
            }
        });
    }

    
    document.addEventListener("submit", function (e) {
        const targetForm = e.target.closest(".comment-form, .reply-form");
        if (targetForm) {
            e.preventDefault(); 
            
            const postId = targetForm.dataset.postId;
            const formData = new FormData(targetForm);

            fetch(targetForm.action, {
                method: "POST",
                headers: {
                    "X-CSRFToken": csrfToken,
                    "X-Requested-With": "XMLHttpRequest"
                },
                body: formData
            })
            .then(res => {
                if (!res.ok) throw new Error(`Network tracking failure response status: ${res.status}`);
                return res.json();
            })
            .then(data => {
                if (!data.success) return;

                targetForm.reset();

                const fallbackMsg = document.querySelector(`#comment-box-${postId} .no-comments-fallback`);
                if (fallbackMsg) fallbackMsg.remove();

                const commentHTML = `
                    <div class="comment-container" id="comment-item-${data.comment_id}" style="margin-bottom: 12px; width: 100%;">
                        <div class="comment-left">
                            <div class="comment-avatar-box" style="width:32px; height:32px; border-radius:50%; overflow:hidden; background:#e4e6eb; display:flex; justify-content:center; align-items:center; flex-shrink:0; margin-right:10px;">
                                <span style="font-size:14px; user-select:none;">👤</span>
                            </div>
                        </div>
                        <div class="comment-right" style="flex: 1; min-width: 0;">
                            <div class="comment-bubble" style="background: #f0f2f5; padding: 8px 12px; border-radius: 18px; display: inline-block; max-width: 100%;">
                                <div class="comment-user" style="font-weight: 700; font-size: 13px; color: #050505; margin-bottom: 4px;">
                                    ${data.username}
                                </div>
                                <div class="comment-text" style="font-size: 15px; color: #050505; line-height: 1.5; word-break: break-word;">
                                    ${data.text}
                                </div>
                            </div>
                            <div class="comment-actions" style="font-size: 12px; color: #65676b; margin-top: 4px; display: flex; gap: 12px; align-items: center; padding-left: 8px;">
                                <span class="comment-time">Just now</span>
                                <button type="button" class="reply-btn" data-comment-id="${data.comment_id}" style="background:none; border:none; color:#65676b; font-weight:bold; cursor:pointer; font-size:12px; padding:0;">
                                    Reply
                                </button>
                                <button type="button" class="delete-comment-btn" data-comment-id="${data.comment_id}" style="background:none; border:none; color:#e41e3f; font-weight:bold; cursor:pointer; font-size:12px; padding:0;">
                                    Delete
                                </button>
                            </div>

                                <div class="reply-list replies-thread-box" id="replies-container-${data.comment_id}" style="margin-left: 12px; margin-top: 8px; border-left: 2px solid #e4e6eb; padding-left: 12px;"></div>
                            <form class="reply-form" method="POST" data-parent="${data.comment_id}" data-post-id="${postId}" action="${targetForm.action}" style="display:none; margin-top: 8px; gap: 8px;">
                                <input type="hidden" name="parent" value="${data.comment_id}">
                                <input type="text" name="text" class="comment-input-field" placeholder="Write a reply..." required style="flex: 1; padding: 6px 12px; border: 1px solid #ced0d4; border-radius: 16px; font-size: 14px; outline:none;">
                                <button type="submit" class="btn-primary" style="background:#1877f2; color:#fff; border:none; padding:6px 12px; border-radius:16px; cursor:pointer; font-size:14px; font-weight:600;">
                                    Reply
                                </button>
                            </form>
                        </div>
                    </div>
                `;

                if (data.parent_id) 
                    {
                    const parentThreadBox = document.getElementById(`replies-container-${data.parent_id}`);
                    if (parentThreadBox) {
                        parentThreadBox.insertAdjacentHTML("beforeend", commentHTML);
                    }
                    targetForm.style.display = "none";
                } 
                else {
                    const targetCommentList = document.getElementById(`comments-list-${postId}`);
                        if (targetCommentList) {
                            targetCommentList.insertAdjacentHTML("beforeend", commentHTML);
                    }
                }

                const labelCounter = document.getElementById(`comments-count-${postId}`);
                if (labelCounter) {
                    labelCounter.innerText = `${data.total_comments} Comments`;
                }
            })
            .catch(err => console.error("Dynamic thread comment execution tracking error:", err));
        }
    });
    
    document.addEventListener("click", function (e) {

        
        if (e.target.closest(".story-item:not(#open-story-popup)")) {
            return; 
        }

        
        const closeCommentBtn = e.target.closest(".close-comment-popup");
        if (closeCommentBtn) {
            e.preventDefault();
            const postId = closeCommentBtn.dataset.postId;
            const targetCommentSection = document.getElementById(`comment-box-${postId}`);
            if (targetCommentSection) {
                targetCommentSection.style.display = "none";
            }
            return;
        }

        
        const likeBtn = e.target.closest(".like-btn");
        if (likeBtn) {
            e.preventDefault();
            const postId = likeBtn.dataset.postId;
            
            fetch(`/post/${postId}/like/`, {
                method: "POST",
                headers: {
                    "X-CSRFToken": csrfToken,
                    "X-Requested-With": "XMLHttpRequest"
                }
            })
            .then(res => res.json())
            .then(data => {
                const countSpan = document.getElementById(`likes-count-${postId}`);
                const textSpan = likeBtn.querySelector(".like-text");
                
                if (countSpan) countSpan.innerText = `👍 ${data.likes_count} Likes`;
                
                if (data.liked) {
                    likeBtn.style.color = "#1877f2";
                    if (textSpan) textSpan.innerText = "Liked";
                } else {
                    likeBtn.style.color = "#65676b";
                    if (textSpan) textSpan.innerText = "Like";
                }
            })
            .catch(err => console.error("Like transaction error:", err));
            return;
        }

        
        const deleteCommentBtn = e.target.closest(".delete-comment-btn");
        if (deleteCommentBtn) {
            e.preventDefault();
            const commentId = deleteCommentBtn.dataset.commentId;
            
            if (!confirm("Are you sure you want to delete this comment?")) return;

            fetch(`/comment/${commentId}/delete/`, {
                method: "POST",
                headers: {
                    "X-CSRFToken": csrfToken,
                    "X-Requested-With": "XMLHttpRequest"
                }
            })
            .then(res => {
                if (!res.ok) throw new Error(`HTTP error! status: ${res.status}`);
                return res.json();
            })
            .then(data => {
                if (data.success) {
                    const commentWrapper = document.getElementById(`comment-item-${commentId}`);
                    if (commentWrapper) {
                        commentWrapper.style.opacity = "0";
                        setTimeout(() => commentWrapper.remove(), 250);
                    }
                    
                    // Automatically trace up to parent card wrapper element to refresh calculations
                    const popupComments = deleteCommentBtn.closest(".popup-content");
                    if (popupComments) {
                        const form = popupComments.querySelector(".comment-form");
                        const postId = form ? form.dataset.postId : null;
                        const labelCounter = document.getElementById(`comments-count-${postId}`);
                        if (labelCounter) labelCounter.innerText = `${data.total_comments} Comments`;
                    }
                } else {
                    alert(data.error || "Unauthorized execution boundary error.");
                }
            })
            .catch(err => console.error("Comment delete execution failure trace:", err));
            return;
        }

        
        const editPostBtn = e.target.closest(".edit-post-btn");
        if (editPostBtn) {
            e.preventDefault();
            const postId = editPostBtn.dataset.postId;
            const captionPara = document.getElementById(`caption-text-${postId}`);
            const oldContent = captionPara ? captionPara.innerText.trim() : "";
            
            const newContent = prompt("Edit your post caption:", oldContent);
            if (newContent === null || newContent.trim() === "") return;

            const formData = new FormData();
            formData.append("content", newContent.trim());

            fetch(`/post/${postId}/edit/`, {
                method: "POST",
                headers: {
                    "X-CSRFToken": csrfToken,
                    "X-Requested-With": "XMLHttpRequest"
                },
                body: formData
            })
            .then(res => res.json())
            .then(data => {
                if (data.success) {
                    if (captionPara) {
                        captionPara.innerText = data.content;
                    } else {
                        const container = document.getElementById(`caption-container-${postId}`);
                        if (container) {
                            container.innerHTML = `<p class="post-caption-text" id="caption-text-${postId}" style="padding-top: 4px;">${data.content}</p>`;
                        }
                    }
                } else {
                    alert(data.error || "Failed to edit post.");
                }
            })
            .catch(err => console.error("Edit post execution error:", err));
            return;
        }

        
        const deletePostBtn = e.target.closest(".delete-post-btn");
        if (deletePostBtn) {
            e.preventDefault();
            const postId = deletePostBtn.dataset.postId;
            
            if (!confirm("Are you completely sure you want to permanently delete this post?")) return;

            fetch(`/post/${postId}/delete/`, {
                method: "POST",
                headers: {
                    "X-CSRFToken": csrfToken,
                    "X-Requested-With": "XMLHttpRequest"
                }
            })
            .then(res => res.json())
            .then(data => {
                if (data.success) {
                    const postCard = deletePostBtn.closest(".post-card");
                    if (postCard) {
                        postCard.style.opacity = "0";
                        setTimeout(() => postCard.remove(), 300);
                    }
                } else {
                    alert(data.error || "Failed to delete post.");
                }
            })
            .catch(err => console.error("Delete post execution error:", err));
            return;
        }

       
        const followBtn = e.target.closest(".follow-btn");
        if (followBtn) {
            e.preventDefault();
            const username = followBtn.dataset.username;
            fetch(`/follows/toggle/${username}/`, {
                method: "POST",
                headers: { "X-CSRFToken": csrfToken, "X-Requested-With": "XMLHttpRequest" }
            })
            .then(res => res.json())
            .then(data => {
                if (!data.success) return;
                document.querySelectorAll(`.follow-btn[data-username="${username}"]`).forEach(btn => {
                    if (data.is_following) {
                        btn.innerHTML = "Following";
                        btn.classList.add("following");
                    } else {
                        btn.innerHTML = "Follow";
                        btn.classList.remove("following");
                    }
                });
            })
            .catch(err => console.error("Follow transaction glitch:", err));
            return;
        }

        
        const commentBtn = e.target.closest(".comment-toggle-btn");
        if (commentBtn) {
            e.preventDefault();
            const postId = commentBtn.dataset.postId;
            const targetCommentSection = document.getElementById(`comment-box-${postId}`);
            if (targetCommentSection) {
                targetCommentSection.style.display = targetCommentSection.style.display === "none" ? "flex" : "none";
            }
            return;
        }

        
        const replyToggleBtn = e.target.closest(".reply-btn");
        if (replyToggleBtn) {
            e.preventDefault();
            const commentId = replyToggleBtn.dataset.commentId;
            const container = document.getElementById(`comment-item-${commentId}`);
            if (container) {
                const targetReplyForm = container.querySelector(".reply-form");
                if (targetReplyForm) {
                    targetReplyForm.style.display = targetReplyForm.style.display === "none" ? "flex" : "none";
                }
            }
            return;
        }
    });
    
    const storyModal = document.getElementById("story-popup-modal");
    const openStoryBtn = document.getElementById("open-story-popup");
    const closeStoryBtn = document.getElementById("close-story-popup");
    const storyImageInput = document.getElementById("story-image-input");
    const storyForm = document.getElementById("ajax-story-form");

  
    if (openStoryBtn && storyModal) {
        openStoryBtn.addEventListener("click", function() {
            storyModal.style.display = "flex";
        });
    }

    
    if (closeStoryBtn && storyModal) {
        closeStoryBtn.addEventListener("click", function() {
            storyModal.style.display = "none";
            if (storyForm) storyForm.reset();
            const previewBox = document.getElementById("story-preview-box");
            if (previewBox) previewBox.style.display = "none";
        });
    }

    
    if (storyImageInput) {
        storyImageInput.addEventListener("change", function() {
            const previewBox = document.getElementById("story-preview-box");
            const previewImage = document.getElementById("story-preview-image");
            
            if (this.files && this.files) {
                const reader = new FileReader();
                reader.onload = function(e) {
                    if (previewImage) previewImage.src = e.target.result;
                    if (previewBox) previewBox.style.display = "block";
                };
                reader.readAsDataURL(this.files);
            }
        });
    }

   
    if (storyForm) {
        storyForm.addEventListener("submit", function(e) {
            e.preventDefault();

            const formData = new FormData(storyForm);
            fetch(storyForm.action, {
                method: "POST",
                headers: {
                    "X-CSRFToken": csrfToken,
                    "X-Requested-With": "XMLHttpRequest"
                },
                body: formData
            })
            .then(res => {
                if (!res.ok) throw new Error("Upload asset transaction failure execution error.");
                return res.json();
            })
            .then(data => {
                if (data.success) {
                    storyModal.style.display = "none";
                    storyForm.reset();
                    const previewBox = document.getElementById("story-preview-box");
                    if (previewBox) previewBox.style.display = "none";

                    
                    const displayAvatar = data.avatar_url ? data.avatar_url : data.image_url;
                    const cleanStoryHTML = `
                        <a href="/stories/${data.story_id}/" class="story-item">
                            <div class="story-circle">
                                <img src="${displayAvatar}" alt="${data.username}">
                            </div>
                            <span>${data.username}</span>
                        </a>
                    `;
                    
                    const openTriggerBtn = document.getElementById("open-story-popup");
                    if (openTriggerBtn) {
                        openTriggerBtn.insertAdjacentHTML("afterend", cleanStoryHTML);
                    }
                    
                    const emptyStoriesFallback = document.querySelector(".empty-stories");
                    if (emptyStoriesFallback) emptyStoriesFallback.remove();
                }
            })
            .catch(err => console.error("Story compilation AJAX transaction failure trace:", err));
        });
    }
