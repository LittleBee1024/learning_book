const RootComponent = {
   data() {
      return {
         counter: 0
      }
   },
   mounted() {
      setInterval(() => {
         this.counter++
      }, 1000)
   }
}

const app = Vue.createApp(RootComponent)
const vm = app.mount('#app')