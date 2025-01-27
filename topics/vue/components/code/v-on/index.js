const RootComponent = {
   methods: {
      say(event, message) {
         // `event` is the native DOM event
         console.log(event.target.tagName)
         alert(message)
      }
   }
}

const vm = Vue.createApp(RootComponent).mount('#app')
