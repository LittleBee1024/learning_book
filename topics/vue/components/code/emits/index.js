const ChildComponent = {
   emits: ['increment-count'],
   template: `
      <div>
         <button @click='this.$emit("increment-count")'>click me</button>
      </div>
   `
}

const RootComponent = {
   data() {
      return {
         count: 0
      }
   },
   methods: {
      increaseCount() {
         this.count++
      }
   },
   components: {
      'child-component': ChildComponent
   }
}

const app = Vue.createApp(RootComponent)

app.mount('#app')
