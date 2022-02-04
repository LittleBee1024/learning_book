const LifeCycle = {
   template: `
      <p>Life Cycle Component</p>
   `,
   beforeCreate() {
      console.log('life cycle function: beforeCreate')
   },
   created() {
      console.log('life cycle function: created')
   },
   beforeMount() {
      console.log('life cycle function: beforeMount')
   },
   mounted() {
      console.log('life cycle function: mounted')
   },
   beforeUpdate() {
      console.log('life cycle function: beforeUpdate')
   },
   updated() {
      console.log('life cycle function: updated')
   },
   activated() {
      console.log('life cycle function: activated')
   },
   deactivated() {
      console.log('life cycle function: deactivated')
   },
   beforeUnmount() {
      console.log('life cycle function: beforeUnmount')
   },
   unmounted() {
      console.log('life cycle function: unmounted')
   },
   errorCaptured() {
      console.log('life cycle function: errorCaptured')
   },
   renderTracked() {
      console.log('life cycle function: renderTracked')
   },
   renderTriggered() {
      console.log('life cycle function: renderTriggered')
   }
}

const RootComponent = {
   data() {
      return {
         active: false
      }
   },
   methods: {
      toggle() {
         this.active = !this.active
      }
   }
}

const app = Vue.createApp(RootComponent)
app.component('life-cycle', LifeCycle)
const vm = app.mount('#app')
